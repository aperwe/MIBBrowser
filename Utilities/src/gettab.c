/*
 *
 * Copyright (C) 1992-1998 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/*
 * gettab.c - program to send get-bulk requests and process get-responses
 * until the entire table has been retrieved.  If a community string is
 * supplied for the clustername field, this degenerates to a getmany
 * This utility needs a copy of snmpinfo.dat in the SR_MGR_CONF_DIR
 * (an environment variable) directory to run.
 */

/*
gettab.c

        Gettab is an SNMP application that retrieves an entire table using
         getbulk requests (or if it is a trivial get, it does a getmany) and
        uses the parts of the response for the next request. The arguments
        are the entity's address, the community string access to the SNMP
        entity or a clustername for a group of snmp Parties and contexts
        (cluster), and the table name is expressed as object identifiers
        in either dot-notation or as the mib-variables as they
        appear in the MIB document. Gettab retrieves chunks of the
        table with each request and uses part of the response to form
        the next request. For instance, running the following:

        gettab -v2 suzzy Guest ifTable         (SNMPv2 clustername)

                      or

        gettab -v[1|2c] suzzy woof ifTable             (SNMPv1 community string)

        would retrieve the interface table.

*/

#include "sr_utils.h"
#include "diag.h"
SR_FILENAME
#include "prnt_lib.h"
#include "oid_lib.h"


static OID *init_oid_ptr = NULL;

#ifdef SR_SNMPv2_PDU
#define MAX_REQS 1000L
#else /* SR_SNMPv2_PDU */
#define MAX_REQS 0L
#endif /* SR_SNMPv2_PDU */

static int request_type;
static int return_value;

static Pdu *do_lookup
    SR_PROTOTYPE((char *tablename));

static void print_response
    SR_PROTOTYPE((VarBind        *resp_vb_ptr,
                  VarBind        *req_vb_ptr,
                  SR_INT32        num_varbinds,
                  SR_INT32        num_columns,
                  OID            *init_oid_ptr));

static Pdu *clone_packet
    SR_PROTOTYPE((VarBind        *resp_vb_ptr,
                  VarBind        *req_vb_ptr,
                  SR_INT32        start_num,
                  SR_INT32        end_num));


extern int EntryPoint_GetTab (int argc, char *argv[]);

extern int EntryPoint_GetTab (int argc, char *argv[])
{
    OID            *oid_ptr = NULL;
    VarBind        *vb_ptr = NULL, *temp_vb_ptr = NULL, *tmp_vb_ptr = NULL;
    Pdu            *req_pdu_ptr = NULL, *resp_pdu_ptr = NULL;
    int             status;
    Pdu            *pdu_ptr, *temp_pdu_ptr;
    Pdu            *copy_pdu_ptr;
    SR_INT32        num_columns;
    SR_INT32        num_varbinds;
    SR_INT32        start_num;
    SR_INT32        end_num;
    SR_INT32        total;
    SR_INT32        pos;
    char           *usage_string;

    total = 0;
    temp_pdu_ptr = NULL;

    usage_string =
        "usage:  %s "
#ifdef SR_SNMPv1_PACKET
        "[-v1] "
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        "[-v2c] "
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv2_PACKET
        "[-v2] "
#endif /* SR_SNMPv2_PACKET */
#ifdef SR_SNMPv3_PACKET
        "[-v3] "
#endif /* SR_SNMPv3_PACKET */
#if (defined(SR_SNMPv2_PACKET) || defined(SR_SNMPv3_PACKET))
        "\\\n\t[-ctxid contextID] [-ctx contextName] "
#endif	/* (defined(SR_SNMPv2_PACKET) || defined(SR_SNMPv3_PACKET)) */
        "\\\n\t[-d] [-timeout seconds] [-retries number] \\\n"
        "\tagent_addr community/userName table_name . . .\n\n"
        "For example, table_name can be:\n"
        "\t\t atTable\n"
        "\t\t ipAddrTable\n"
        "\t\t ipRouteTable\n"
        "\t\t ipNetToMediaTable\n"
        "\t\t tcpConnTable\n"
        "\t\t udpTable\n"
        "\t\t egpNeighTable\n"
#ifdef SR_SYS_OR_TABLE
        "\t\t snmpORTable\n"
#endif /* SR_SYS_OR_TABLE */
#ifdef SR_SNMPv1_WRAPPER
        "\t\t srCommunityTable\n"
#endif /* SR_SNMPv1_WRAPPER */
#ifdef SR_SNMPv2_PACKET
        "\t\t userNameTable\n"
#endif /* SR_SNMPv2_PACKET */
#ifdef SR_SNMPv2_PACKET
        "\t\t usmUserTable\n"
#endif /* SR_SNMPv2_PACKET */
#ifdef SR_SNMPv2_ADMIN
        "\t\t v2ContextTable\n"
        "\t\t viewTreeTable\n"
        "\t\t acTable\n"
        "\t\t transportTable\n"
        "\t\t notifyTable\n"
        "\t\t notifyInformParametersTable\n"
        "\t\t proxyForwardingTable\n"
#endif /* SR_SNMPv2_ADMIN */
#ifdef SR_SNMPv3_ADMIN
        "\t\t vacmSecurityToGroupTable\n"
        "\t\t vacmAccessTable\n"
#endif /* SR_SNMPv3_ADMIN */
        "\n";

    if (InitializeUtility(SNMP_COMMUNICATIONS, &argc, argv,
                          NULL, NULL, usage_string)) {
        return_value = -1;
        goto finished;
    }

    if (argc != 2) {
        fprintf(stderr, usage_string, argv[0]);
        return_value = -1;
        goto finished;
    }


#ifndef SR_SNMPv2_PDU
    request_type = GET_NEXT_REQUEST_TYPE;
#else /* SR_SNMPv2_PDU */
#ifdef SR_SNMPv1_PACKET
    if (util_version == SR_SNMPv1_VERSION) {
        request_type = GET_NEXT_REQUEST_TYPE;
    } else {
        request_type = GET_BULK_REQUEST_TYPE;
    }
#else /* SR_SNMPv1_PACKET */
    request_type = GET_BULK_REQUEST_TYPE;
#endif /* SR_SNMPv1_PACKET */
#endif /* SR_SNMPv2_PDU */

    /*
     * Find all elements of this tablename and put them into a varbind_list
     */
    if ((req_pdu_ptr = do_lookup(argv[1])) ==
        NULL) {
        fprintf(stderr, "gettab: do_lookup failed\n");
        exit(-1);
    }

    init_oid_ptr = MakeOIDFromDot(argv[1]);
    if (init_oid_ptr == (OID *) NULL) {
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
        CloseUp();
        exit(-1);
    }

    num_columns = req_pdu_ptr->num_varbinds;

    copy_pdu_ptr = MakePdu(request_type, ++global_req_id, 0L,
                            MAX_REQS, NULL, NULL, 0L, 0L, 0L);
    if (copy_pdu_ptr == (Pdu *) NULL) {
        FreeOID(init_oid_ptr);
        init_oid_ptr = NULL;
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
        CloseUp();
        exit(-1);
    }

    temp_vb_ptr = req_pdu_ptr->var_bind_list;

    while (temp_vb_ptr != NULL) {
        oid_ptr = MakeOID(temp_vb_ptr->name->oid_ptr,
                           temp_vb_ptr->name->length);
        vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
        FreeOID(oid_ptr);
        oid_ptr = NULL;
        LinkVarBind(copy_pdu_ptr, vb_ptr);
        temp_vb_ptr = temp_vb_ptr->next_var;
    }

    if (BuildPdu(copy_pdu_ptr) == -1) {
      DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
      goto finished;
    }
    resp_pdu_ptr = NULL;

    while (1) {

    retries = tretries;
  retry_snmp_request:
        /* Perform SNMP request */
        resp_pdu_ptr = PerformRequest(req_pdu_ptr, &status);
        if (status == SR_TIMEOUT) {
            retries--;
            if (retries > 0) {
                DPRINTF((APALWAYS, "retrying . . .\n"));
                goto retry_snmp_request;
            }
            DPRINTF((APALWAYS, "giving up . . .\n"));
            goto finished;
        }
        if (status == SR_ERROR) {
            return_value = -1;
            goto finished;
        }

        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;

            if (resp_pdu_ptr->type != GET_RESPONSE_TYPE) {
                fprintf(stderr, "received non GET_RESPONSE_TYPE packet.  Exiting.\n");
                exit(-1);
            }

            /* check for error status stuff... */
            if (resp_pdu_ptr->u.normpdu.error_status != NO_ERROR) {
                if (resp_pdu_ptr->u.normpdu.error_status == NO_ACCESS_ERROR) {
                    printf("End of MIB.\n");
                    exit(0);
                }
                PrintErrorCode(resp_pdu_ptr->u.normpdu.error_index,
                                 resp_pdu_ptr->u.normpdu.error_status);

                exit(-1);
            }                        /* if ! NOERROR (was an error) */
            else {                /* no error was found */

                /*
                 * Check for termination case (only checking first one for
                 * now)
                 */
                if (CheckOID(init_oid_ptr, resp_pdu_ptr->var_bind_list->name) < 0) {
                    exit(0);
                }

                if (resp_pdu_ptr->num_varbinds >= num_columns) {
                    num_varbinds = resp_pdu_ptr->num_varbinds -
                        (resp_pdu_ptr->num_varbinds % num_columns);

                    print_response(resp_pdu_ptr->var_bind_list,
                     copy_pdu_ptr->var_bind_list, num_varbinds, num_columns,
                                   init_oid_ptr);

                    /* build up new request packet here */
                    global_req_id++;

                    start_num = num_varbinds - num_columns -
                        (num_varbinds % num_columns);
                    end_num = start_num + num_columns;
                    if ((pdu_ptr = clone_packet(resp_pdu_ptr->var_bind_list,
                            copy_pdu_ptr->var_bind_list, start_num, end_num
                                   )) == (Pdu *) NULL) {
                        fprintf(stderr, "Cannot clone packet\n");
                        exit(-1);
                    }
                    req_pdu_ptr = pdu_ptr;
                    num_columns = req_pdu_ptr->num_varbinds;
                    pdu_ptr = NULL;
                }                /* if (resp_pdu_ptr->num_varbinds >
                                 * num_columns) */
                else {
                    /*
                     * If this is the beginning of a row, start building a
                     * pdu for the next row request.
                     */
                    if (temp_pdu_ptr == NULL) {
                        total = resp_pdu_ptr->num_varbinds;
                        print_response(resp_pdu_ptr->var_bind_list,
                                       copy_pdu_ptr->var_bind_list, total,
                                       total, init_oid_ptr);

                        /*
                         * For now, plug in req_id for the request id.  When
                         * this pdu is actually used, the req_id will be
                         * updated correctly.
                         */
                        temp_pdu_ptr = MakePdu(request_type, global_req_id, 0L,
                                          MAX_REQS, NULL, NULL, 0L, 0L, 0L);

                        if (temp_pdu_ptr == NULL) {
                            fprintf(stderr, "Cannot make temp_pdu_ptr\n");
                            exit(-1);
                        }

                        temp_vb_ptr = resp_pdu_ptr->var_bind_list;
                        tmp_vb_ptr = copy_pdu_ptr->var_bind_list;

                        while ((temp_vb_ptr != NULL) && (tmp_vb_ptr != NULL)) {
                            /*
                             * If what we asked for does not match what we
                             * got, substitute what we asked for for the next
                             * pdu request.
                             */
                            if (CmpOIDClass(temp_vb_ptr->name,
                                              tmp_vb_ptr->name) != 0) {
                                oid_ptr = MakeOID(tmp_vb_ptr->name->oid_ptr,
                                                   tmp_vb_ptr->name->length);
                            }
                            /*
                             * Otherwise put in what we received in the last
                             * packet.
                             */
                            else {
                                oid_ptr = MakeOID(temp_vb_ptr->name->oid_ptr,
                                                 temp_vb_ptr->name->length);
                            }
                            vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
                            FreeOID(oid_ptr);
                            oid_ptr = NULL;
                            LinkVarBind(temp_pdu_ptr, vb_ptr);
                            temp_vb_ptr = temp_vb_ptr->next_var;
                            tmp_vb_ptr = tmp_vb_ptr->next_var;
                        }

                        /*
                         * Formulate the next request pdu (the rest of this
                         * row)
                         */
                        global_req_id++;
                        temp_vb_ptr = req_pdu_ptr->var_bind_list;
                        pos = 0;

                        while ((temp_vb_ptr != NULL) && (pos < total)) {
                            temp_vb_ptr = temp_vb_ptr->next_var;
                            pos++;
                        }

                        if (pos != total) {
                            fprintf(stderr, "null varbind pointer\n");
                            exit(-1);
                        }

                        pdu_ptr = MakePdu(request_type, global_req_id, 0L, 0L,
                                           NULL, NULL, 0L, 0L, 0L);

                        if (pdu_ptr == NULL) {
                            fprintf(stderr, "could not make pdu_ptr\n");
                            exit(-1);
                        }

                        while (temp_vb_ptr != NULL) {
                            oid_ptr = MakeOID(temp_vb_ptr->name->oid_ptr,
                                               temp_vb_ptr->name->length);
                            vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
                            FreeOID(oid_ptr);
                            oid_ptr = NULL;
                            LinkVarBind(pdu_ptr, vb_ptr);
                            temp_vb_ptr = temp_vb_ptr->next_var;
                        }

                        if (BuildPdu(pdu_ptr) == -1) {
                          DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
                          goto finished;
                        }

                        FreePdu(resp_pdu_ptr);
                        resp_pdu_ptr = NULL;
                        FreePdu(req_pdu_ptr);
                        req_pdu_ptr = NULL;
                        req_pdu_ptr = pdu_ptr;
                        pdu_ptr = NULL;
                    }                /* if (temp_pdu_ptr == NULL) */

                    else {
                        /*
                         * Get to the correct varbind of copy_pdu_ptr
                         */
                        pos = 0;
                        tmp_vb_ptr = copy_pdu_ptr->var_bind_list;

                        while ((pos < total) && (tmp_vb_ptr != NULL)) {
                            tmp_vb_ptr = tmp_vb_ptr->next_var;
                            pos++;
                        }
                        if (pos != total) {
                            fprintf(stderr, "null varbind pointer 2\n");
                            exit(-1);
                        }

                        print_response(resp_pdu_ptr->var_bind_list,
                                     tmp_vb_ptr, resp_pdu_ptr->num_varbinds,
                                  resp_pdu_ptr->num_varbinds, init_oid_ptr);

                        total += resp_pdu_ptr->num_varbinds;

                        /*
                         * Add varbinds to temp_pdu_ptr (next full row
                         * request)
                         */
                        temp_vb_ptr = resp_pdu_ptr->var_bind_list;

                        while ((temp_vb_ptr != NULL) && (tmp_vb_ptr != NULL)) {
                            /*
                             * If what we asked for does not match what we
                             * got, substitute what we asked for for the next
                             * pdu request.
                             */
                            if (CmpOIDClass(temp_vb_ptr->name,
                                              tmp_vb_ptr->name) != 0) {
                                oid_ptr = MakeOID(tmp_vb_ptr->name->oid_ptr,
                                                   tmp_vb_ptr->name->length);
                            }
                            /*
                             * Otherwise put in what we received in the last
                             * packet.
                             */
                            else {
                                oid_ptr = MakeOID(temp_vb_ptr->name->oid_ptr,
                                                 temp_vb_ptr->name->length);
                            }
                            vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
                            FreeOID(oid_ptr);
                            oid_ptr = NULL;
                            LinkVarBind(temp_pdu_ptr, vb_ptr);
                            temp_vb_ptr = temp_vb_ptr->next_var;
                            tmp_vb_ptr = tmp_vb_ptr->next_var;
                        }

                        /*
                         * If total == num_columns, we now have a full row
                         * for the next request pdu.
                         */
                        global_req_id++;

                        if (total == num_columns) {
                            FreePdu(resp_pdu_ptr);
                            resp_pdu_ptr = NULL;
                            FreePdu(req_pdu_ptr);
                            req_pdu_ptr = NULL;
                            if (BuildPdu(temp_pdu_ptr) == -1) {
                              DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
                              goto finished;
                            }
                            if ((pdu_ptr = clone_packet(temp_pdu_ptr->var_bind_list,
                                     copy_pdu_ptr->var_bind_list, 0L, total
                                   )) == (Pdu *) NULL) {
                                fprintf(stderr, "cannot clone temp_pdu_ptr\n");
                                exit(-1);
                            }
                            total = 0;
                            req_pdu_ptr = pdu_ptr;
                            FreePdu(temp_pdu_ptr);
                            temp_pdu_ptr = NULL;
                            pdu_ptr = NULL;
                            num_columns = req_pdu_ptr->num_varbinds;
                        }

                        /*
                         * Otherwise, ask for the part of the row that was
                         * not received.
                         */
                        else {
                            temp_vb_ptr = req_pdu_ptr->var_bind_list;
                            tmp_vb_ptr = copy_pdu_ptr->var_bind_list;
                            pos = 0;

                            while ((tmp_vb_ptr != NULL) && (pos < total)) {
                                tmp_vb_ptr = tmp_vb_ptr->next_var;
                                pos++;
                            }
                            if (pos != total) {
                                fprintf(stderr, "null varbind pointer\n");
                                exit(-1);
                            }
                            pos = 0;
                            while ((temp_vb_ptr != NULL) &&
                                   (pos < resp_pdu_ptr->num_varbinds)) {
                                temp_vb_ptr = temp_vb_ptr->next_var;
                                pos++;
                            }
                            if (pos != resp_pdu_ptr->num_varbinds) {
                                fprintf(stderr, "null varbind pointer\n");
                                exit(-1);
                            }

                            pdu_ptr = MakePdu(request_type, global_req_id, 0L, 0L,
                                               NULL, NULL, 0L, 0L, 0L);

                            if (pdu_ptr == NULL) {
                                fprintf(stderr, "could not make pdu_ptr\n");
                                exit(-1);
                            }

                            while (temp_vb_ptr != NULL) {
                                oid_ptr = MakeOID(temp_vb_ptr->name->oid_ptr,
                                                 temp_vb_ptr->name->length);
                                vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
                                FreeOID(oid_ptr);
                                oid_ptr = NULL;
                                LinkVarBind(pdu_ptr, vb_ptr);
                                temp_vb_ptr = temp_vb_ptr->next_var;
                            }

                            BuildPdu(pdu_ptr);
                            if (BuildPdu(pdu_ptr) == -1) {
                              DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
                              goto finished;
                            }

                            FreePdu(resp_pdu_ptr);
                            resp_pdu_ptr = NULL;
                            FreePdu(req_pdu_ptr);
                            req_pdu_ptr = NULL;
                            req_pdu_ptr = pdu_ptr;
                            pdu_ptr = NULL;
                        }        /* else (total != num_columns) */
                    }                /* else (temp_pdu_ptr != NULL) */

                }                /* else  (num_varbinds < num_columns) */

            }                        /* end of else no error */

    }                                /* while(1) */

  finished:
    return return_value;
}                                /* main */


/*
 * Look up the tablename in snmpinfo.dat.  If it exists and is an
 * Aggregate (table), collect all entries in the table into a varbind_list.
 * This could be a GET-NEXT (trivial) or a GET-BULK request.
 */
static Pdu            *
do_lookup(tablename)
    char           *tablename;
{

    OID            *oid_ptr;
    OID            *tableOID, *nameOID;
    VarBind        *vb_ptr;
    Pdu            *req_pdu_ptr;
    FILE           *fp;
    int             found;
    char            name[100];
    char            oid_val[100];
    char            type[40];
    char            access[40];
    char            buffer[256];


    oid_ptr = (OID *) NULL;
    tableOID = (OID *) NULL;
    nameOID = (OID *) NULL;
    vb_ptr = (VarBind *) NULL;
    req_pdu_ptr = (Pdu *) NULL;
    fp = NULL;
    found = FALSE;

    req_pdu_ptr = MakePdu(request_type, global_req_id, 0L, MAX_REQS,
                           NULL, NULL, 0L, 0L, 0L);

    /*
     * Open file, search for the tablename specified in snmpinfo.dat
     */

    if ((fp = fopen(fn_snmpinfo_dat, "r")) == NULL) {
        fprintf(stderr, "do_lookup: could not open snmpinfo.dat\n");
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
        return (NULL);
    }

    while ((found == FALSE) && (fgets(buffer, 256, fp) != NULL)) {
        if (sscanf(buffer, "%s %s %s %s", name, oid_val, type,
                          access) != 4) {
            continue;
        }

        if ((strcmp(tablename, name) == 0) && (strcmp(type, "Aggregate") ==
                                               0)) {
            found = TRUE;
        }
    }

    if (found == FALSE) {
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
        return (NULL);
    }

    /*
     * Now search for every column in this conceptual row and add it to the
     * pdu.
     */

    found = FALSE;
    while ((found == FALSE) && (fgets(buffer, 256, fp) != NULL)) {
        if (sscanf(buffer, "%s %s %s %s", name, oid_val, type,
                          access) != 4) {
            continue;
        }
        if ((strcmp(type, "Aggregate") != 0) &&
            (strcmp(access, "not-accessible") != 0)) {
            found = TRUE;
        }
    }

    if (found == FALSE) {
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
        return (NULL);
    }

    if ((oid_ptr = MakeOIDFromDot(oid_val)) ==
        (OID *) NULL) {
        fprintf(stderr, "do_lookup: cannot make oid_ptr\n");
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
        return (NULL);
    }

    if ((vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL)) == (VarBind *) NULL) {
        fprintf(stderr, "do_lookup: cannot make vb_ptr\n");
        FreeOID(oid_ptr);
        oid_ptr = NULL;
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
        return (NULL);
    }
    FreeOID(oid_ptr);
    oid_ptr = NULL;

    if (LinkVarBind(req_pdu_ptr, vb_ptr) == (short) -1) {
        fprintf(stderr, "do_lookup: could not link varbind\n");
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
        FreeVarBind(vb_ptr);
        vb_ptr = NULL;
        return (NULL);
    }
    vb_ptr = NULL;

    while (fgets(buffer, 256, fp) != NULL) {
        if ( sscanf(buffer, "%s %s %s %s", name, oid_val, type,
                          access) != 4) {
            continue;
        }

        if (strcmp(access, "not-accessible") != 0) {
            /*
             * Add this table element to the varbind_list
             */
            if ((tableOID = MakeOIDFromDot(tablename)) ==
                (OID *) NULL) {
                fprintf(stderr, "do_lookup: Could not make tableOID\n");
                FreePdu(req_pdu_ptr);
                req_pdu_ptr = NULL;
                return (NULL);
            }
            if ((nameOID = MakeOIDFromDot(name)) == (OID *) NULL) {
                fprintf(stderr, "do_lookup: Could not make nameOID\n");
                FreeOID(tableOID);
                tableOID = NULL;
                FreePdu(req_pdu_ptr);
                req_pdu_ptr = NULL;
                return (NULL);
            }
            if (CmpOIDClass(tableOID, nameOID) == 0) {
                if ((oid_ptr = MakeOIDFromDot(oid_val)) == (OID *) NULL) {
                    fprintf(stderr, "do_lookup: cannot make oid_ptr 2\n");
                    FreePdu(req_pdu_ptr);
                    req_pdu_ptr = NULL;
                    FreeOID(tableOID);
                    tableOID = NULL;
                    FreeOID(nameOID);
                    nameOID = NULL;
                    return (NULL);
                }

                if ((vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL)) ==
                    (VarBind *) NULL) {
                    fprintf(stderr, "do_lookup: cannot make vb_ptr 2\n");
                    FreeOID(oid_ptr);
                    oid_ptr = NULL;
                    FreePdu(req_pdu_ptr);
                    req_pdu_ptr = NULL;
                    FreeOID(tableOID);
                    tableOID = NULL;
                    FreeOID(nameOID);
                    nameOID = NULL;
                    return (NULL);
                }
                FreeOID(oid_ptr);
                oid_ptr = NULL;

                if (LinkVarBind(req_pdu_ptr, vb_ptr) == (short) -1) {
                    fprintf(stderr, "do_lookup: could not link varbind 2\n");
                    FreePdu(req_pdu_ptr);
                    req_pdu_ptr = NULL;
                    FreeOID(tableOID);
                    tableOID = NULL;
                    FreeOID(nameOID);
                    nameOID = NULL;
                    FreeVarBind(vb_ptr);
                    vb_ptr = NULL;
                    return (NULL);
                }
                vb_ptr = NULL;
            }                        /* if CmpOIDClass(tableOID, ..... ) */

            else {                /* Have gotten all of the elements of the
                                 * table */
                FreeOID(tableOID);
                tableOID = NULL;
                FreeOID(nameOID);
                nameOID = NULL;
                break;
            }
            FreeOID(tableOID);
            tableOID = NULL;
            FreeOID(nameOID);
            nameOID = NULL;
        }                        /* if (strcmp(access, "not-accessible") != 0) */
    }                                /* while (fgets(....) != NULL */

    if (BuildPdu(req_pdu_ptr) == -1) {
      DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
      return NULL;
    }

    return (req_pdu_ptr);

}                                /* do_lookup() */


/*
print_response

        This routine prints out the contents of a varbind list in human readable
form from the first varbind to the number specified (num_varbinds) or until
resp_vb_ptr->next_var or req_vb_ptr->next_var points to NULL, whichever comes
first.
*/

static void
print_response(resp_vb_ptr, req_vb_ptr, num_varbinds, num_columns, init_oid_ptr)
    VarBind        *resp_vb_ptr;
    VarBind        *req_vb_ptr;
    SR_INT32        num_varbinds;
    SR_INT32        num_columns;
    OID            *init_oid_ptr;
{

    VarBind        *vb_ptr, *req_vb;
    VarBind        *temp_vb_ptr, *temp_req_vb;
    char            buffer[128];
    SR_INT32        num;
    SR_INT32        pos;
    SR_INT32        num_exceptions;
    int             terminate;

    terminate = TRUE;
    num = 0;
    vb_ptr = resp_vb_ptr;
    req_vb = req_vb_ptr;

    if ((vb_ptr == NULL) || (req_vb == NULL)) {
        DPRINTF((APWARN, "print_response: vb_ptr and/or req_vb_ptr is NULL\n"));
        exit(-1);
    }

    while ((vb_ptr != NULL) && (req_vb != NULL) && (num < num_varbinds)) {
        /*
         * Check for termination case.  If all elements of this row do not
         * match what was requested, exit.  Check this at the beginning of
         * each row.  Also check to see if there is an
         * END_OF_MIB_VIEW_EXCEPTION for each element in the row. If there
         * is, exit.
         */
        if ((num % num_columns) == 0) {
            temp_vb_ptr = vb_ptr;
            temp_req_vb = req_vb;
            pos = 0;
            while ((terminate == TRUE) && (temp_vb_ptr != NULL) &&
                   (temp_req_vb != NULL) && (pos < num_columns)) {
                if (CmpOIDClass(temp_vb_ptr->name, temp_req_vb->name) == 0) {
                    terminate = FALSE;
                }
                temp_vb_ptr = temp_vb_ptr->next_var;
                temp_req_vb = temp_req_vb->next_var;
                pos++;
            }
            if (terminate == TRUE) {
                DPRINTF((APTRACE, "print_response: Have finished table retrieval\n"));
                exit(-1);
            }

#ifdef SR_SNMPv2_PDU
            temp_vb_ptr = vb_ptr;
            temp_req_vb = req_vb;
            num_exceptions = 0;
            pos = 0;
            while ((temp_vb_ptr != NULL) && (temp_req_vb != NULL) &&
                   (pos < num_columns)) {

                if (temp_vb_ptr != NULL) {
                    if (CmpOIDClass(temp_vb_ptr->name, temp_req_vb->name) != 0) {
                        num_exceptions++;
                    }
                    else if (temp_vb_ptr->value.type != END_OF_MIB_VIEW_EXCEPTION) {
                        break;
                    }
                    else if (temp_vb_ptr->value.type == END_OF_MIB_VIEW_EXCEPTION) {
                        num_exceptions++;
                    }

                }
                temp_vb_ptr = temp_vb_ptr->next_var;
                temp_req_vb = temp_req_vb->next_var;
                pos++;
            }
            temp_vb_ptr = NULL;
            temp_req_vb = NULL;

            if (num_exceptions == num_columns) {
                printf("End of MIB.\n");
                exit(0);
            }
#endif                                /* SR_SNMPv2_PDU */

        }

        /*
         * If the table is finished (i.e., we are into another group or table
         * in the MIB), we can exit.
         *
         *    As it turns out, not really.  If the last column of the table
         *    is not implemented, one will get an object from beyond the 
         *    end of the table, and, therefore, gettab will print out only
         *    the first row of a table.
         *
         * if (CheckOID(init_oid_ptr, vb_ptr->name) != 0) {
         *    DPRINTF((APTRACE, "Have reached the end of the table\n"));
         *    exit(-1);
         * }
         */

        buffer[0] = '\0';

        if (CmpOIDClass(vb_ptr->name, req_vb->name) != 0) {
            if (MakeDotFromOID(req_vb->name, buffer) == -1) {
                DPRINTF((APWARN, "print_response: req_vb->name\n"));
                exit(-1);
            }
            printf("%s = Unavailable\n", buffer);
        }

        else {
            if (MakeDotFromOID(vb_ptr->name, buffer) == -1) {
                DPRINTF((APWARN, "print_response: vb_ptr->name\n"));
                exit(-1);
            }
            printf("%s = ", buffer);
            switch (vb_ptr->value.type) {
            case COUNTER_TYPE:        /* handle unsigned integers includes
                                 * COUNTER_32 */
            case GAUGE_TYPE:        /* includes GAUGE_32 */
            case TIME_TICKS_TYPE:

                printf(UINT32_FORMAT"\n", vb_ptr->value.ul_value);

                break;
            case INTEGER_TYPE:        /* handle signed integers includes INTEGER_32 */

                printf("%s\n", GetType(buffer, vb_ptr->value.sl_value));
                break;
            case IP_ADDR_PRIM_TYPE:
                printf("%d.%d.%d.%d\n",
                       vb_ptr->value.os_value->octet_ptr[0],
                       vb_ptr->value.os_value->octet_ptr[1],
                       vb_ptr->value.os_value->octet_ptr[2],
                       vb_ptr->value.os_value->octet_ptr[3]);
                break;
            case OBJECT_ID_TYPE:
                if ( MakeDotFromOID(vb_ptr->value.oid_value, buffer) == -1) {
                    DPRINTF((APWARN, "print_response: vb_ptr->value.oid_value:\n"));
                    exit(-1);
                }
                printf("%s\n", buffer);
                break;
            case OCTET_PRIM_TYPE:        /* handle quasi-octet strings */
            case OPAQUE_PRIM_TYPE:
                if (PrintAscii(vb_ptr->value.os_value) < 0) {        /* if cannot print ascii */
                    PrintOctetString(vb_ptr->value.os_value, 16);
                }
                break;
            case NULL_TYPE:
                printf("NULL\n");
                break;

#ifdef  SR_SNMPv2_PDU
            case COUNTER_64_TYPE:
                printf("0x%x%x\n", (int)(vb_ptr->value.uint64_value->big_end),
                       (int)(vb_ptr->value.uint64_value->little_end));
                break;
            case NO_SUCH_OBJECT_EXCEPTION:
                printf("NO_SUCH_OBJECT_EXCEPTION\n");
                break;
            case NO_SUCH_INSTANCE_EXCEPTION:
                printf("NO_SUCH_INSTANCE_EXCEPTION\n");
                break;
            case END_OF_MIB_VIEW_EXCEPTION:
                printf("END_OF_MIB_VIEW_EXCEPTION\n");
                break;
#endif                                /* SR_SNMPv2_PDU */

            default:
                DPRINTF((APWARN, "print_varbind: Illegal type: 0x%x\n", vb_ptr->value.type));

                exit(-1);

                break;
            };                        /* end of switch */
        }                        /* else requested var and received var match */

        num++;
        vb_ptr = vb_ptr->next_var;
        if ((num % num_columns) == 0) {
            req_vb = req_vb_ptr;
            terminate = TRUE;
            printf("\n");
        }
        else {
            req_vb = req_vb->next_var;
        }

    }                                /* while ..... */

}                                /* print_response() */

/*
clone_packet

        This routine makes a new pdu and copies each varbind in the
varbind_list(resp_vb_ptr) until the ending point is reached or until
resp_vb_ptr->next_var or req_vb_ptr->next_var is NULL, whichever comes first.
*/
static Pdu            *
clone_packet(resp_vb_ptr, req_vb_ptr, start_num, end_num)
    VarBind        *resp_vb_ptr;
    VarBind        *req_vb_ptr;
    SR_INT32        start_num;
    SR_INT32        end_num;
{

    SR_INT32        num;
    OID            *oid_ptr;
    VarBind        *resp_vb, *req_vb;
    VarBind        *temp_vb_ptr;
    Pdu            *pdu_ptr;

    num = 0;
    pdu_ptr = NULL;
    resp_vb = resp_vb_ptr;
    req_vb = req_vb_ptr;

    if ((resp_vb_ptr == NULL) || (req_vb_ptr == NULL)) {
        fprintf(stderr, "clone_packet: resp_vb_ptr and/or req_vb_ptr is NULL\n");
        return (NULL);
    }

    pdu_ptr = MakePdu(request_type, global_req_id, 0L, MAX_REQS, NULL,
                       NULL, 0L, 0L, 0L);
    if (pdu_ptr == NULL) {
        fprintf(stderr, "clone_packet: pdu_ptr is NULL\n");
        return (NULL);
    }

    while ((resp_vb != NULL) && (num < start_num)) {
        num++;
        resp_vb = resp_vb->next_var;
    }

    while ((resp_vb != NULL) && (req_vb != NULL) && (num < end_num)) {

        /*
         * If what we asked for does not match what we got, substitute what we
         * asked for for the next pdu request.
         */
        if (CmpOIDClass(resp_vb->name, req_vb->name) != 0) {
            if ((oid_ptr = MakeOID(req_vb->name->oid_ptr,
                                    req_vb->name->length)) == (OID *) NULL) {
                fprintf(stderr, "clone_packet: could not make oid_ptr\n");
                FreePdu(pdu_ptr);
                pdu_ptr = NULL;
                return (NULL);
            }
        }
        /*
         * Otherwise put in what we received in the last packet.
         */
        else {
            if ((oid_ptr = MakeOID(resp_vb->name->oid_ptr,
                                  resp_vb->name->length)) == (OID *) NULL) {
                fprintf(stderr, "clone_packet: could not make oid_ptr 2\n");
                FreePdu(pdu_ptr);
                pdu_ptr = NULL;
                return (NULL);
            }
        }

        temp_vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);

        if (temp_vb_ptr == NULL) {
            fprintf(stderr, "clone_packet: could not make temp_vb_ptr\n");
            FreeOID(oid_ptr);
            oid_ptr = NULL;
            FreePdu(pdu_ptr);
            pdu_ptr = NULL;
            return (NULL);
        }
        FreeOID(oid_ptr);
        oid_ptr = NULL;

        if (LinkVarBind(pdu_ptr, temp_vb_ptr) == (short) -1) {
            fprintf(stderr, "clone_packet: could not link varbind\n");
            FreePdu(pdu_ptr);
            pdu_ptr = NULL;
            FreeVarBind(temp_vb_ptr);
            temp_vb_ptr = NULL;
            return (NULL);
        }
        temp_vb_ptr = NULL;
        req_vb = req_vb->next_var;
        resp_vb = resp_vb->next_var;
        num++;
    }                                /* while ..... */

    if (BuildPdu(pdu_ptr) == -1) {
      DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
      return NULL;
    }

    return (pdu_ptr);

}                                /* clone_packet() */

