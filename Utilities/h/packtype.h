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

/* $Id: packtype.h,v 1.10 1998/01/08 02:34:11 mark Exp $ */

#ifndef SR_PACKTYPE_H
#define SR_PACKTYPE_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct DESCRIPTOR_S DESCRIPTOR;
typedef struct SELECTOR_S SELECTOR;
typedef struct TRANS_TABLE_S TRANS_TABLE;
typedef struct ATTR_S ATTR;
typedef struct PACK_DESCRIPTOR_S PACK_DESCRIPTOR;
typedef struct PackParams_s PackParams;

typedef struct PointerCacheElement_s {
    char *key;
    char *value;
} PointerCacheElement;

typedef struct PointerCache_s {
    int nelems;
    int maxelems;
    PointerCacheElement *elements;
} PointerCache;

#define FLAG_SELECTED    ((SR_INT32)0x01)
#define FLAG_OVERLAP     ((SR_INT32)0x02)
#define FLAG_POINTER     ((SR_INT32)0x04)
#define FLAG_COUNTED     ((SR_INT32)0x08)
#define FLAG_NULLTERM    ((SR_INT32)0x10)
#define FLAG_POINTER_CACHE ((SR_INT32)0x20)

#define BT_VOID            ((SR_INT32)1)
#define BT_INT8            ((SR_INT32)2)
#define BT_UINT8           ((SR_INT32)3)
#define BT_INT16           ((SR_INT32)4)
#define BT_UINT16          ((SR_INT32)5)
#define BT_INT32           ((SR_INT32)6)
#define BT_UINT32          ((SR_INT32)7)
#define BT_OPAQUE_POINTER  ((SR_INT32)8)
#define BT_UNUSED_POINTER  ((SR_INT32)9)

struct SELECTOR_S {
    unsigned long mask;
    long value;
};

struct DESCRIPTOR_S {
    SR_INT32 type; /* index into class array, or - for base type */
    SR_INT32 flags; /* flag values are:
                * 
                *   FLAG_SELECTED
                *       is this a member of a descriminated union
                * 
                *   FLAG_OVERLAP
                *       does the next field overlap me
                * 
                *   FLAG_POINTER
                *       are the contents of this member pointer(s)
                * 
                *   FLAG_COUNTED
                *       is this object counted by some other object
                * 
                *   FLAG_NULLTERM
                *       this object points to a null terminated array
                * 
                */
    SR_INT32 array_length; /* number of instances embedded in this structure */
    SR_INT32 counter; /* offset to counter descriptor */
/*#define number counter */ /* constant number of objects pointed to */
    SR_INT32 selector;
    SELECTOR *selectors;
};

#ifndef TYPE_COMPILER
#ifdef PACK_C
#define PACK_VOID ((SR_INT32)1)
#define PACK_INT8 ((SR_INT32)2)
#define PACK_UINT8 ((SR_INT32)3)
#define PACK_INT16 ((SR_INT32)4)
#define PACK_UINT16 ((SR_INT32)5)
#define PACK_INT32 ((SR_INT32)6)
#define PACK_UINT32 ((SR_INT32)7)
#define PACK_OPAQUE_POINTER ((SR_INT32)8)
#define PACK_UNUSED_POINTER ((SR_INT32)9)
#define FIRST_CLASS 10 /* changes if number of basic types change */
#endif /* PACK_C */

/* shared between attr flags and pps flags */
#define SR_BIG_ENDIAN    ((SR_INT32)0x00)
#define SR_LITTLE_ENDIAN ((SR_INT32)0x01)
#define ENDIAN(x) ((x) & SR_LITTLE_ENDIAN)


#define SR_PTR_POINTER   ((SR_INT32)0x00)
#define SR_PTR_OFFSET    ((SR_INT32)0x10)

#define PTRTYPE(x) ((x) & SR_PTR_OFFSET)

/* Flags to be passed to the Pack() function */
#define SR_UNPACKING  0x00
#define SR_PACKING    0x01
#define SR_DO_TRANS   0x00
#define SR_NO_TRANS   0x02

struct TRANS_TABLE_S {
    char *ttable;
    SR_INT32 table_len;
};

struct PACK_DESCRIPTOR_S {
    DESCRIPTOR **classes;
    ATTR *attr1;
    ATTR *attr2;
    TRANS_TABLE *ttable;
};

struct PackParams_s {
    DESCRIPTOR **classes;
    ATTR *src_attr; /* int/pointer alignment/size info in source structure */
    ATTR *dst_attr; /* int/pointer alignment/size info in destination struct */
    char *src_progenitor; /* pointer to the top level object to be packed */
    char *dst_progenitor; /* always stays the same on recursive calls */
    unsigned long base_offset;
    SR_INT32 flags;
    TRANS_TABLE *ttable; 
    PointerCache *cache;
};

struct ATTR_S {
    SR_INT32 align_POINTER;
    SR_INT32 size_POINTER;
    SR_INT32 flags;
    SR_INT32 strictest_alignment;
    SR_INT32 *class_aligns;
    SR_INT32 *class_sizes;
    SR_INT32 **member_offsets;
};

/* pack.c */

PACK_DESCRIPTOR *create_pack_descriptor
    SR_PROTOTYPE((DESCRIPTOR **classes,
		  ATTR *attr1,
		  ATTR *attr2));

void free_pack_descriptor
    SR_PROTOTYPE((PACK_DESCRIPTOR *pd));


ATTR *create_local_attr
    SR_PROTOTYPE((DESCRIPTOR *classes[]));


ATTR *create_attr
    SR_PROTOTYPE((DESCRIPTOR *classes[],
		  SR_INT32 align_INT8,
		  SR_INT32 align_INT16,
		  SR_INT32 align_INT32,
		  SR_INT32 align_POINTER,
		  SR_INT32 size_POINTER,
		  SR_INT32 flags));

void free_attr
    SR_PROTOTYPE((DESCRIPTOR *classes[],
		  ATTR *attr));

SR_INT32 Pack
    SR_PROTOTYPE((PACK_DESCRIPTOR *pd,
		  SR_INT32 flags,
		  char *ptr,
		  unsigned long base_offset,
		  SR_INT32 num,
		  SR_INT32 type,
		  char *buffer));

SR_INT32 endian
    SR_PROTOTYPE((void));

SR_INT32 Int8_align
    SR_PROTOTYPE((void));

SR_INT32 Int16_align
    SR_PROTOTYPE((void));

SR_INT32 Int32_align
    SR_PROTOTYPE((void));

SR_INT32 Pointer_align
    SR_PROTOTYPE((void));

#endif /* TYPE_COMPILER */

#ifdef  __cplusplus
}
#endif

#endif /* SR_PACKTYPE_H */
