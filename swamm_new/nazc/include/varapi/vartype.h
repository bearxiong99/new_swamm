#ifndef __VAR_TYPE_H__
#define __VAR_TYPE_H__

#include "typedef.h"
#include "vardef.h"

typedef struct	_tagMIBValue
{
		OID3		oid;							// OID3
		int			type;							// Data Type
		int			len;							// Length
		union
		{
			signed char     s8;                  	// 1 Byte Value
            BYTE            u8;                  	// 1 Byte Value
            signed short    s16;                  	// 2 Byte Value
            WORD            u16;                 	// 2 Byte Value
            int             s32;                	// 4 Byte Value
            UINT            u32;                	// 4 Byte Value
			OID3			id;						// 3 Byte OID
            char            *p;                 	// Pointer
		}	stream;
		struct	_tagMIBValue 	*pNext;				// Link
}	MIBValue, *PMIBValue;

typedef struct	_tagVARITEM							// Save Structure
{
		OID3		oid;							// Object ID
		time_t		lLastModified;					// Object Last Modified
		BYTE		type;							// Object Type
		WORD		len;							// Object Length
		union
		{
			signed char     s8;               		// 1 Byte Value
            BYTE            u8;                		// 1 Byte Value
            signed short    s16;              	 	// 2 Byte Value
            WORD            u16;               		// 2 Byte Value
            int             s32;              		// 4 Byte Value
            UINT            u32;              		// 4 Byte Value
            char            *p;               		// Pointer
			OID3			id;						// 3 Byte OID
			BYTE			ip[4];					// IPv4
			EUI64			eui64;					// EUI64 ID
			TIMESTAMP		time;					// Timestamp
			TIMEDATE		day;					// Timedate
			char			str[256];				// String
		}	stream;		
}	__attribute__ ((packed)) VARITEM, *PVARITEM;

typedef int (*PFNVARHANDLER)(struct _tagVAROBJECT *pObject, int nMethod, MIBValue *pValue,
								MIBValue **pList, int *nCount,
								MIBValue *pReference);

typedef struct	_tagVAROBJECT
{
		BYTE					id;					// Value OID Number
		const char					*pszName;			// Name
		MIBValue				var;				// Value
		BYTE					attr;				// Attribute
		struct	_tagVAROBJECT	*pChild;			// Child Node
		struct	_tagVAROBJECT	*pGroup;			// Group Node
		PFNVARHANDLER			pfnHandler;			// Query Handler	
		time_t					lLastModified;		// Last Modify Time
		BOOL					bChanged;			// Change Flag
}	VAROBJECT, *PVAROBJECT;

typedef void (*PFNENUMVAROBJECT)(VAROBJECT *pObject, void *pParam);

#endif	// __VAR_TYPE_H__
