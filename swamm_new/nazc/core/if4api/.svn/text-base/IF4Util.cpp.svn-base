#include "common.h"
#include "if4frame.h"
#include "varapi.h"
#include "Locker.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "logdef.h"

CLocker	m_TidLocker;
BYTE	m_theClientTID = 0;

#define	OID_ACCEPT		"0123456789."

#define COMMAND_LOG( ...)      UpdateLogFile(LOG_DIR, CMD_HISTORY_LOG_FILE, 0, FALSE, __VA_ARGS__)

BYTE BCD_BYTE(BYTE c)
{
	BYTE	high, low;
	
	high = (BYTE)((c/10) * 10);
	low	 = c % 10;
	return (BYTE)(high + low);
}

BYTE BCD2BYTE(BYTE c)
{
	BYTE	high, low;

	high = (BYTE)((c >> 4) * 10);
	low	 = ((c & 0xf) % 10);
	return (BYTE)(high + low);
}

WORD BCD_WORD(WORD value)
{
	WORD	v;
	BYTE	b1, b2, b3, b4;

	b1	= (value % 1000) % 10;
	b2	= (value / 100) % 10;
	b3	= (value / 10) % 10;
	b4	= value % 10;

	v = b1*1000 + b2*100 + b3*10 + b4;
	return v;	
}

void BCD_TIMESTAMP(TIMESTAMP *dest, TIMESTAMP *src)
{
	memset(dest, 0, sizeof(TIMESTAMP));
	dest->year	= BCD_WORD(src->year);
	dest->mon	= BCD_BYTE(src->mon);
	dest->day	= BCD_BYTE(src->day);
	dest->hour	= BCD_BYTE(src->hour);
	dest->min	= BCD_BYTE(src->min);
	dest->sec	= BCD_BYTE(src->sec);
}

void GET_TIMESTAMP(TIMESTAMP *pStamp, time_t *pNow)
{
    time_t          now, cur;
    struct  tm      when;

    if (!pStamp)
        return;

    if (pNow == NULL)
    {
        time(&now);
        pNow = &now;
    }
    else
    {   
        cur = *pNow; 
        if (*pNow == 0)
        {   
            memset(pStamp, 0, sizeof(TIMESTAMP));
            return;
        }
    }
    when            = *localtime(pNow);
    pStamp->year    = when.tm_year + 1900;
    pStamp->mon     = when.tm_mon + 1;
    pStamp->day     = when.tm_mday;
    pStamp->hour    = when.tm_hour;
    pStamp->min     = when.tm_min;
    pStamp->sec     = when.tm_sec;
}

BYTE GetTID()
{
	BYTE	tid;

	m_TidLocker.Lock();
	tid = m_theClientTID++;
	m_TidLocker.Unlock();

	return tid;
}

BOOL StringToOid(const char *pszOid, OID3 *pOid)
{
    return VARAPI_StringToOid(pszOid, pOid);
}

BOOL OidToString(OID3 *pOid, char *pszOid)
{
	if (!pOid || !pszOid)
		return FALSE;

	if (pOid->id1 == 0)
		 sprintf(pszOid, "0.0.0");
	else if (pOid->id2 == 0)
		 sprintf(pszOid, "%0d", pOid->id1);
	else if (pOid->id3 == 0)
		 sprintf(pszOid, "%0d.%0d", pOid->id1, pOid->id2);
	else sprintf(pszOid, "%0d.%0d.%0d", pOid->id1, pOid->id2, pOid->id3);
	return TRUE;
}

void FreeLinkedValue(MIBValue *pStart)
{
	MIBValue	*pValue, *pFree;

	pValue = pStart;
	while(pValue)
	{
		pFree  = pValue;
		pValue = pValue->pNext;
		VARAPI_FreeValue(pFree);
	}
}

const char *GetDataFileType(int nType)
{
	switch(nType) {
	  case IF4_DATAFILE_GENERIC :		return "General File";
	  case IF4_DATAFILE_MEASUREMENT :	return "Measurement File";
	  case IF4_DATAFILE_SYSTEM :		return "System File";
	  case IF4_DATAFILE_APPLICATION :	return "Application File";
	  case IF4_DATAFILE_CONFIG :		return "Configuration File";
	}
	return "Unknown File";
}

void DumpSmiValue(char *pValue, int nCount)
{
	int			i, nSeek = 0, len;
	char		szOID[30];
	char		*pszValue;
	TIMESTAMP	*t;
	TIMEDATE	*d;
	SMIValue	*p;
	VAROBJECT	*pObject;
	BYTE		type;

	for(i=0; i<nCount; i++)
	{
		p 		= (SMIValue *)(pValue + nSeek);
		pObject = VARAPI_GetObject(&p->oid);
		type	= pObject ? pObject->var.type : 0;
		OidToString(&p->oid, szOID);

		if (p->len > 0)
		{
			XDEBUG("%4d: %s(%s), %s(%0d), LEN=%0d, VALUE=", 
				i+1,
				pObject ? pObject->pszName : "Unknown",
				szOID,
				VARAPI_GetTypeName(type),
				type,
				p->len);

		    switch(type) {
 		      case VARSMI_BOOL :
				   XDEBUG("%s(%0d)\xd\xa",
							p->stream.u8 == 0 ? "FALSE" : "TRUE", 
							p->stream.u8);
				   break;
  		      case VARSMI_BYTE :	XDEBUG("%0d\xd\xa", p->stream.u8); break;
  		      case VARSMI_WORD :	XDEBUG("%0d\xd\xa", p->stream.u16); break;
		      case VARSMI_UINT :	XDEBUG("%0d\xd\xa", p->stream.u32); break;
		      case VARSMI_CHAR :	XDEBUG("%0d\xd\xa", p->stream.s8); break;
 		      case VARSMI_SHORT :	XDEBUG("%0d\xd\xa", (signed short)p->stream.s16); break;
 		      case VARSMI_INT : 	XDEBUG("%0d\xd\xa", (signed int)p->stream.s32); break;
		      case VARSMI_OID :
				   XDEBUG("%0d.%0d.%0d\xd\xa",
							(BYTE)p->stream.id.id1,
							(BYTE)p->stream.id.id2,
							(BYTE)p->stream.id.id3);
				   break;
  		      case VARSMI_IPADDR :
				   XDEBUG("%0d.%0d.%0d.%0d\xd\xa",
							(BYTE)p->stream.str[0],
							(BYTE)p->stream.str[1],
							(BYTE)p->stream.str[2],
							(BYTE)p->stream.str[3]);
				   break;
		      case VARSMI_TIMESTAMP :
				   t = (TIMESTAMP *)p->stream.str;
				   XDEBUG("%04d/%02d/%02d %02d:%02d:%02d\xd\xa",
							t->year, t->mon, t->day,
							t->hour, t->min, t->sec);
				   break;
 		      case VARSMI_TIMEDATE :
				   d = (TIMEDATE *)p->stream.str;
				   XDEBUG("%04d/%02d/%02d\xd\xa", d->year, d->mon, d->day);
				   break;
  		      case VARSMI_STRING :
				   len = MIN(p->len, 45);
				   pszValue = (char *)MALLOC(len+1);
				   if (pszValue != NULL)
				   {
				   	   memcpy(pszValue, p->stream.str, len);
				   	   pszValue[len] = 0;
				   	   XDEBUG("'%s'\xd\xa", pszValue);
					   FREE(pszValue);
				   }
				   break;
 		      case VARSMI_UNKNOWN :
  		      case VARSMI_EUI64 :
		      case VARSMI_STREAM :
 		      case VARSMI_OPAQUE :
				   XDUMP(p->stream.str, MIN(p->len, 15));
				   break;
		    }
		}
		else
		{
			XDEBUG("%4d: %s(%s), %s(%0d), LEN=%0d\xd\xa", 
				i+1,
				pObject ? pObject->pszName : "Unknown",
				szOID,
				VARAPI_GetTypeName(type),
				type,
				p->len);
		}
		nSeek += (p->len + 5);
	}
}

void SmiToHostValue(char *pValue, int nCount)
{
#if     defined(__BIG_ENDIAN_SYSTEM__)
	int			i, nSeek = 0;
	char		szOID[30];
	SMIValue	*p;
	VAROBJECT	*pObject;
	BYTE		type;

	for(i=0; i<nCount; i++)
	{
		p 		= (SMIValue *)(pValue + nSeek);
		pObject = VARAPI_GetObject(&p->oid);
		type	= pObject ? pObject->var.type : 0;
		OidToString(&p->oid, szOID);

        p->len = LittleToHostShort(p->len);

		if (p->len > 0)
		{
		    switch(type) {
 		      case VARSMI_BOOL :
  		      case VARSMI_BYTE :	
		      case VARSMI_CHAR :	
		      case VARSMI_OID :
  		      case VARSMI_IPADDR :
  		      case VARSMI_STRING :
 		      case VARSMI_UNKNOWN :
  		      case VARSMI_EUI64 :
		      case VARSMI_STREAM :
 		      case VARSMI_OPAQUE :
				   break;
		      case VARSMI_TIMESTAMP :
                   TIMESTAMP * t = (TIMESTAMP *) p->stream.p;
                   t->year = HostToLittleShort(t->year);
				   break;
  		      case VARSMI_SHORT :	p->stream.u16 = (signed short) LittleToHostShort(p->stream.u16);
				   break;
  		      case VARSMI_WORD :	p->stream.u16 = (WORD) LittleToHostShort(p->stream.u16);
				   break;
		      case VARSMI_UINT :	p->stream.u32 = (UINT) LittleToHostInt(p->stream.u32);
				   break;
		      case VARSMI_INT :	    p->stream.u32 =  LittleToHostInt(p->stream.u32);
				   break;
		    }
		}
		nSeek += (p->len + 5);
	}
#endif
}

void SmiToLittleValue(char *pValue, int nCount)
{
#if     defined(__BIG_ENDIAN_SYSTEM__)
	int			i, nSeek = 0;
	char		szOID[30];
	SMIValue	*p;
	VAROBJECT	*pObject;
	BYTE		type;

	for(i=0; i<nCount; i++)
	{
		p 		= (SMIValue *)(pValue + nSeek);
		pObject = VARAPI_GetObject(&p->oid);
		type	= pObject ? pObject->var.type : 0;
		OidToString(&p->oid, szOID);

		if (p->len > 0)
		{
		    switch(type) {
 		      case VARSMI_BOOL :
  		      case VARSMI_BYTE :	
		      case VARSMI_CHAR :	
		      case VARSMI_OID :
  		      case VARSMI_IPADDR :
  		      case VARSMI_STRING :
 		      case VARSMI_UNKNOWN :
  		      case VARSMI_EUI64 :
		      case VARSMI_STREAM :
 		      case VARSMI_OPAQUE :
				   break;
		      case VARSMI_TIMESTAMP :
                   TIMESTAMP * t = (TIMESTAMP *) p->stream.p;
                   t->year = HostToLittleShort(t->year);
				   break;
  		      case VARSMI_SHORT :	p->stream.u16 = (signed short) HostToLittleShort(p->stream.u16);
				   break;
  		      case VARSMI_WORD :	p->stream.u16 = (WORD) HostToLittleShort(p->stream.u16);
				   break;
		      case VARSMI_UINT :	p->stream.u32 = (UINT) HostToLittleInt(p->stream.u32);
				   break;
		      case VARSMI_INT :	    p->stream.u32 =  HostToLittleInt(p->stream.u32);
				   break;
		    }
		}
		nSeek += (p->len + 5);
        p->len = HostToLittleShort(p->len);
	}
#endif
}

const char *GetIF4ErrorMessage(int nError)
{
	switch(nError) {
	  case IF4ERR_NOERROR :				return "OK";
	  case IF4ERR_INVALID_ID :			return "Invalid ID";
	  case IF4ERR_INVALID_OID :			return "Invalid OID";
	  case IF4ERR_INVALID_INDEX :		return "Invalid Index";
	  case IF4ERR_INVALID_VALUE :		return "Invalid Value";
	  case IF4ERR_INVALID_ACCOUNT :		return "Invalid Account";
	  case IF4ERR_INVALID_PASSWORD :	return "Invalid Password";
	  case IF4ERR_INVALID_LENGTH :		return "Invalid Length";
	  case IF4ERR_INVALID_TYPE :		return "Invalid Type";
	  case IF4ERR_INVALID_COUNT :		return "Invalid Count";
	  case IF4ERR_INVALID_PARAM :		return "Invalid Parameter";
	  case IF4ERR_INVALID_TARGET :		return "Invalid Target";
	  case IF4ERR_INVALID_HOST :		return "Invalid Host";
	  case IF4ERR_INVALID_HANDLE :		return "Invalid Handle";
	  case IF4ERR_INVALID_FORMAT :		return "Invalid Format";
	  case IF4ERR_INVALID_ADDRESS :		return "Invalid Address";
	  case IF4ERR_INVALID_PORT :		return "Invalid Port";
	  case IF4ERR_INVALID_BUFFER :		return "Invalid Buffer";
	  case IF4ERR_INVALID_PROTOCOL :	return "Invalid Protocol";
	  case IF4ERR_INVALID_GROUP :		return "Invalid Group";
	  case IF4ERR_INVALID_NAME :		return "Invalid Name";
	  case IF4ERR_INVALID_FILENAME :	return "Invalid FileName";
	  case IF4ERR_INVALID_COMMAND :		return "Invalid Command";
	  case IF4ERR_INVALID_CHANNEL :		return "Invalid Channel";
	  case IF4ERR_INVALID_PANID :		return "Invalid PANID";
	  case IF4ERR_INVALID_SINK :		return "Invalid SINK";
	  case IF4ERR_INVALID_INVOKE :		return "Invalid Invoke";
	  case IF4ERR_INVALID_EVENT :		return "Invalid EVENT";
	  case IF4ERR_INVALID_MEMBER :		return "Invalid Member";
	  case IF4ERR_INVALID_APN :			return "Invalid APN";
      case IF4ERR_INVALID_URL :         return "Invalid URL";
      case IF4ERR_INVALID_VERSION :     return "Invalid Version";
      case IF4ERR_INVALID_TRIGGERID :   return "Invalid Trigger ID";
      case IF4ERR_INVALID_OTA_TYPE :    return "Invalid OTA type";
      case IF4ERR_INVALID_MODEL :       return "Invalid Model";
      case IF4ERR_INVALID_TRANSFER_TYPE :return "Invalid Transfer type";
      case IF4ERR_INVALID_OTA_STEP :    return "Invalid OTA step";
      case IF4ERR_INVALID_BIN_URL :     return "Invalid Binary URL";
      case IF4ERR_INVALID_BIN_CHECKSUM :return "Invalid Binary Checksum";
      case IF4ERR_INVALID_DIFF_URL :    return "Invalid Diff URL";
      case IF4ERR_INVALID_DIFF_CHECKSUM :return "Invalid Diff Checksum";
	  case IF4ERR_INVALID_TRANSACTION_ID : return "Invalid Transaction ID";
	  case IF4ERR_CANNOT_ACCEPT :		return "Server does not accept data";
	  case IF4ERR_BUSY_METERING :		return "Active metering (BUSY)";
	  case IF4ERR_BUSY_RECOVERY :		return "Active recovery (BUSY)";
	  case IF4ERR_BUSY_TIMESYNC :		return "Active timesync (BUSY)";
	  case IF4ERR_NO_ENTRY :			return "No Entry";
	  case IF4ERR_CANNOT_DELETE :     	return "Cannot Delete";
	  case IF4ERR_OUT_OF_RANGE :		return "Out Of Range";
	  case IF4ERR_EXCPTION :			return "Excption Error";
	  case IF4ERR_OUT_OF_BINDING :		return "Out of binding";
	  case IF4ERR_INTERNAL_ERROR :		return "Internal error";
	  case IF4ERR_DELIVERY_ERROR :		return "Delivery error";
	  case IF4ERR_ZIGBEE_NETWORK_ERROR : return "Zigbee network error";
	  case IF4ERR_NOT_READY :			return "Not Ready";
	  case IF4ERR_CANNOT_CONNECT :		return "Cannot Connect";
	  case IF4ERR_METER_READING_ERROR :	return "Meter Reading Error";
	  case IF4ERR_TIMEOUT :				return "Timeout";
	  case IF4ERR_CRC_ERROR :			return "CRC Error";
	  case IF4ERR_DUPLICATE :			return "Duplicate";
	  case IF4ERR_CANNOT_CHANGE :		return "Cannot Change";
	  case IF4ERR_ALREADY_INITALIZED :	return "Already Initialized";
	  case IF4ERR_BUSY :				return "Busy";
	  case IF4ERR_SYSTEM_ERROR :		return "System Error";
	  case IF4ERR_IO_ERROR :			return "I/O Error";
	  case IF4ERR_MEMORY_ERROR :		return "Memeory Error";
	  case IF4ERR_DO_NOT_SUPPORT :		return "Do not support";
	  case IF4ERR_CANNOT_GET :			return "Cannot Get Operation";
	  case IF4ERR_CANNOT_SET :          return "Cannot Set Operation";
	  case IF4ERR_CANNOT_GETLIST :      return "Cannot List Operation";
	  case IF4ERR_CANNOT_SUPPORT_MULTI : return "Cannot Support Multi Query";
	  case IF4ERR_NEED_MORE_VALUE :		return "Need More Parameter Value";
	  case IF4ERR_NO_MORE_WORKER :		return "No More Worker";
      case IF4ERR_DUPLICATE_TRIGGERID : return "Duplicate Trigger ID";
      case IF4ERR_TRIGGERID_NOT_FOUND : return "Trigger ID not found";
      case IF4ERR_USER_MODIFY:          return "User Modification";
      case IF4ERR_TRANSACTION_CREATE_FAIL:  return "Transaction Create Fail";
      case IF4ERR_TRANSACTION_UPDATE_FAIL:  return "Transaction Update Fail";
      case IF4ERR_TRANSACTION_DELETE_FAIL:  return "Transaction Delete Fail";
      case IF4ERR_DEPRECATED_FUNCTION:  return "Deprecated Function";
	}
	return "Unknown Error";
}

void DumpMIBValue(MIBValue *pValue, int nCount)
{
	int			i, len;
	char		szOID[30];
	char		*pszValue;
	TIMESTAMP	*t;
	TIMEDATE	*d;
	MIBValue	*p;
	VAROBJECT	*pObject;
	BYTE		type;

	p = pValue;
	for(i=0; i<nCount && p; i++)
	{
		pObject = VARAPI_GetObject(&p->oid);
		type	= pObject ? pObject->var.type : p->type;
		OidToString(&p->oid, szOID);

		if (p->len > 0)
		{
			XDEBUG("%4d: %s(%s), %s(%0d), LEN=%0d, VALUE=", 
				i+1, pObject ? pObject->pszName : "Unknown",
				szOID, VARAPI_GetTypeName(type), type, p->len);

		    switch(type) {
 		      case VARSMI_BOOL : 	XDEBUG("%s(%0d)\xd\xa", p->stream.u8 == 0 ? "FALSE" : "TRUE", p->stream.u8); break;
  		      case VARSMI_BYTE :	XDEBUG("%0d\xd\xa", p->stream.u8); break;
  		      case VARSMI_WORD :	XDEBUG("%0d\xd\xa", p->stream.u16); break;
		      case VARSMI_UINT :	XDEBUG("%0d\xd\xa", p->stream.u32); break;
		      case VARSMI_CHAR :	XDEBUG("%0d\xd\xa", p->stream.s8); break;
 		      case VARSMI_SHORT :	XDEBUG("%0d\xd\xa", (signed short)p->stream.s16); break;
 		      case VARSMI_INT : 	XDEBUG("%0d\xd\xa", (signed int)p->stream.s32); break;
		      case VARSMI_OID : 	XDEBUG("%0d.%0d.%0d\xd\xa", p->stream.id.id1, p->stream.id.id2, p->stream.id.id3); break;
  		      case VARSMI_IPADDR : 	XDEBUG("%0d.%0d.%0d.%0d\xd\xa",
											p->stream.p[0], p->stream.p[1],
											p->stream.p[2], p->stream.p[3]);
				   					break;
		      case VARSMI_TIMESTAMP :
				   t = (TIMESTAMP *)p->stream.p;
				   XDEBUG("%04d/%02d/%02d %02d:%02d:%02d\xd\xa",
							t->year, t->mon, t->day,
							t->hour, t->min, t->sec);
				   break;
 		      case VARSMI_TIMEDATE :
				   d = (TIMEDATE *)p->stream.p;
				   XDEBUG("%04d/%02d/%02d\xd\xa", d->year, d->mon, d->day);
				   break;
  		      case VARSMI_STRING :
				   len = MIN(p->len, 45);
				   pszValue = (char *)MALLOC(len+1);
				   if (pszValue != NULL)
				   {
				   	   memcpy(pszValue, p->stream.p, len);
				   	   pszValue[len] = 0;
				   	   XDEBUG("'%s'\xd\xa", pszValue);
					   FREE(pszValue);
				   }
				   break;
 		      case VARSMI_UNKNOWN :
  		      case VARSMI_EUI64 :
		      case VARSMI_STREAM :
 		      case VARSMI_OPAQUE :
				   XDUMP(p->stream.p, MIN(p->len, 15));
				   break;
		    }
		}
		else
		{
			XDEBUG("%4d: %s(%s), %s(%0d), LEN=%0d\xd\xa", 
				i+1, pObject ? pObject->pszName : "Unknown",
				szOID, VARAPI_GetTypeName(type), type, p->len);
		}
		p = p->pNext;
	}
}

void LogSmiValue(char *pValue, int nCount)
{
	int			i, nSeek = 0, len;
	char		szOID[30];
	char		*pszValue;
	TIMESTAMP	*t;
	TIMEDATE	*d;
	SMIValue	*p;
	VAROBJECT	*pObject;
	BYTE		type;

	for(i=0; i<nCount; i++)
	{
		p 		= (SMIValue *)(pValue + nSeek);
		pObject = VARAPI_GetObject(&p->oid);
		type	= pObject ? pObject->var.type : 0;
		OidToString(&p->oid, szOID);

		if (p->len > 0)
		{
			COMMAND_LOG("%4d: %s=", i+1, pObject ? pObject->pszName : "Unknown");

		    switch(type) {
 		      case VARSMI_BOOL :
				   COMMAND_LOG("%s(%0d)\xd\xa",
							p->stream.u8 == 0 ? "FALSE" : "TRUE", 
							p->stream.u8);
				   break;
  		      case VARSMI_BYTE :	COMMAND_LOG("%0d\xd\xa", p->stream.u8); break;
  		      case VARSMI_WORD :	COMMAND_LOG("%0d\xd\xa", p->stream.u16); break;
		      case VARSMI_UINT :	COMMAND_LOG("%0d\xd\xa", p->stream.u32); break;
		      case VARSMI_CHAR :	COMMAND_LOG("%0d\xd\xa", p->stream.s8); break;
 		      case VARSMI_SHORT :	COMMAND_LOG("%0d\xd\xa", (signed short)p->stream.s16); break;
 		      case VARSMI_INT : 	COMMAND_LOG("%0d\xd\xa", (signed int)p->stream.s32); break;
		      case VARSMI_OID :
				   COMMAND_LOG("%0d.%0d.%0d\xd\xa",
							(BYTE)p->stream.id.id1,
							(BYTE)p->stream.id.id2,
							(BYTE)p->stream.id.id3);
				   break;
  		      case VARSMI_IPADDR :
				   COMMAND_LOG("%0d.%0d.%0d.%0d\xd\xa",
							(BYTE)p->stream.str[0],
							(BYTE)p->stream.str[1],
							(BYTE)p->stream.str[2],
							(BYTE)p->stream.str[3]);
				   break;
		      case VARSMI_TIMESTAMP :
				   t = (TIMESTAMP *)p->stream.str;
				   COMMAND_LOG("%04d/%02d/%02d %02d:%02d:%02d\xd\xa",
							t->year, t->mon, t->day,
							t->hour, t->min, t->sec);
				   break;
 		      case VARSMI_TIMEDATE :
				   d = (TIMEDATE *)p->stream.str;
				   COMMAND_LOG("%04d/%02d/%02d\xd\xa", d->year, d->mon, d->day);
				   break;
  		      case VARSMI_STRING :
				   len = MIN(p->len, 45);
				   pszValue = (char *)MALLOC(len+1);
				   if (pszValue != NULL)
				   {
				   	   memcpy(pszValue, p->stream.str, len);
				   	   pszValue[len] = 0;
				   	   COMMAND_LOG("'%s'\xd\xa", pszValue);
					   FREE(pszValue);
				   }
				   break;
 		      case VARSMI_UNKNOWN :
  		      case VARSMI_EUI64 :
		      case VARSMI_STREAM :
 		      case VARSMI_OPAQUE :
				   FILEDUMP(CMD_HISTORY_LOG_FILE, p->stream.str, MIN(p->len, 15), FALSE, FALSE);
				   break;
		    }
		}
		else
		{
			COMMAND_LOG("%4d: %s(%s), %s(%0d), LEN=%0d\xd\xa", 
				i+1,
				pObject ? pObject->pszName : "Unknown",
				szOID,
				VARAPI_GetTypeName(type),
				type,
				p->len);
		}
		nSeek += (p->len + 5);
	}
}
