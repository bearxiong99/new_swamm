#ifndef __OID_MIB_H__
#define __OID_MIB_H__

#define ATTR_SUM_LENGTH			0x1000
#define ATTR_GROUP				0x2000
#define ATTR_DOOR				0x4000
#define ATTR_END				0x8000
#define ATTR_COMMAND			0x0100
#define ATTR_EVENT				0x0200
#define ATTR_ALARM				0x0400
#define ATTR_SMI				0x0800
#define ATTR_READ				0x0001
#define ATTR_WRITE				0x0002
#define ATTR_LIST				0x0004

typedef struct	_tagOBJECT
{
		OID3	oid;								// Variable OID
		BYTE	id;									// Sigle ID
		int		type;								// Variable Type
		int		len;								// Length
		char	*pszName;							// Name
		char	*pszDescr;							// Description
		int		line;								// Line Number
		int		attr;								// Attribute
		struct	_tagOBJECT		*pChild;			// Child Object
		struct	_tagOBJECT		*pNext;				// Next Object
		struct	_tagOBJECT		*pLast;				// Last Object
		struct	_tagOBJECT		*pGroup;			// Last Object
		OID3	group;								// GROUP
}	OBJECT;

#endif	// __OID_MIB_H__
