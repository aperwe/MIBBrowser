int EntryPoint_GetOne (int argc, char *argv[])
{
	int             return_value = 0;
	OID            *oid_ptr = NULL;
	VarBind        *vb_ptr = NULL;
	Pdu            *req_pdu_ptr = NULL, *resp_pdu_ptr = NULL;
	int             num_requests;
	int             i;
	int             status;
///////////////////////
/// Modifications here.
///////////////////////


	oid_ptr = MakeOIDFromDot (argv[i + 1]);
	/*
	 *  Pathological case: argument is "iso" or "1".  We turn this
	 *  into 1.0 as a convenience to the user.
	 */
	if (oid_ptr != NULL)
	{
		if (oid_ptr->length == 1)
		{
			if (oid_ptr->oid_ptr[0] == 1)
			{
				FreeOID (oid_ptr);
				oid_ptr = MakeOIDFromDot ("1.0");
			}
			else
			{
				FreeOID (oid_ptr);
				oid_ptr = NULL;
			}
		}
	}
	if (oid_ptr == NULL)
	{
//			fprintf(stderr, "Cannot translate variable class: %s\n", argv[i + 1]);
		sprintf (pszOutputBuffer, "Cannot translate variable class: %s\n", argv[i + 1]); SendOutput ();
		return_value = -1;
		goto finished;
	}
	vb_ptr = MakeVarBindWithNull (oid_ptr, (OID *) NULL);
	FreeOID (oid_ptr);
	oid_ptr = NULL;

	/* COPY VarBind into PDU */
	LinkVarBind (req_pdu_ptr, vb_ptr);

	vb_ptr = NULL;


	/* Make the PDU packlet */
	if (BuildPdu (req_pdu_ptr) == -1)
	{
		DPRINTF ((APALWAYS, "BuildPdu failed.  Giving up.\n"));
		goto finished;
	}

	retries = tretries;
	retry_snmp_request:
	/* Perform SNMP request */
	resp_pdu_ptr = PerformRequest (req_pdu_ptr, &status);
	if (status == SR_TIMEOUT)
	{
		retries--;
		if (retries > 0)
		{
			DPRINTF ((APALWAYS, "retrying . . .\n"));
			goto retry_snmp_request;
		}
		DPRINTF ((APALWAYS, "giving up . . .\n"));
		goto finished;
	}
	if (status == SR_ERROR)
	{
		return_value = -1;
		goto finished;
	}

	FreePdu (req_pdu_ptr);
	req_pdu_ptr = NULL;

	if (resp_pdu_ptr->type != GET_RESPONSE_TYPE)
	{
//		fprintf(stderr, "received non GET_RESPONSE_TYPE packet.  Exiting.\n");
		sprintf (pszOutputBuffer, "received non GET_RESPONSE_TYPE packet.  Exiting.\n"); SendOutput ();
		return_value = -1;
		goto finished;
	}

	/* check for error status stuff... */
	if (resp_pdu_ptr->u.normpdu.error_status != NO_ERROR)
	{
		PrintErrorCode (resp_pdu_ptr->u.normpdu.error_index, resp_pdu_ptr->u.normpdu.error_status);

		return_value = -1;
		goto finished;
	}				/* if ! NOERROR (was an error) */
	else
	{			/* no error was found */
		PrintVarBindList (resp_pdu_ptr->var_bind_list);
	}			/* end of else no error */


  finished:
	if (resp_pdu_ptr != NULL)
	{
		FreePdu (resp_pdu_ptr);
		resp_pdu_ptr = NULL;
	}
	CloseUp ();
	return return_value;
}				/* getone.c */

