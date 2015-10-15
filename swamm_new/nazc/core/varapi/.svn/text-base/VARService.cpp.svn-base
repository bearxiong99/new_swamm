#include "common.h"
#include <sys/stat.h>
#include "mcudef.h"
#include "VARService.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

//////////////////////////////////////////////////////////////////////
// Data Definitions
//////////////////////////////////////////////////////////////////////

CVARService		*m_pVARService = NULL;
extern BOOL		m_bGlobalEnableModify;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVARService::CVARService()
{
	m_pVARService = this;
	m_pRootObject = NULL;
	*m_szFileName = '\0';
	m_bSave		  = TRUE;
	m_bModified	  = FALSE;
}

CVARService::~CVARService()
{
}

//////////////////////////////////////////////////////////////////////
// Service Startup/Shutdown
//////////////////////////////////////////////////////////////////////

int CVARService::Initialize(const char *pszFileName, VAROBJECT *pRoot, BOOL bSave)
{
	m_bSave	= bSave;
	m_pRootObject = pRoot;

	if (pRoot != NULL)
		InitObjectValue(pRoot);

	strcpy(m_szFileName, pszFileName ? pszFileName : "");
	if (*m_szFileName) Load();
	
	StartupThread(3);
	return VARERR_NOERROR;
}

int CVARService::Destroy()
{
	if (m_szFileName || *m_szFileName)
		Save(TRUE);

	ShutdownThread();
	if (m_pRootObject != NULL)
		FreeObjectValue(m_pRootObject);

	return VARERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// CVARService Operations
//////////////////////////////////////////////////////////////////////

void CVARService::SetModify(BOOL bModified)
{
	m_IOLocker.Lock();
	m_bModified	= bModified;
	ActiveThread();
	m_IOLocker.Unlock();
}

int CVARService::Load()
{
	struct stat file_info;
	FILE	*fp;
	int		nResult;
	int		nNewLength = 0;
	int		nOldLength = 0;
    char    buffer[1024];

	m_IOLocker.Lock();

	nResult = stat(m_szFileName, &file_info);
	if (nResult != -1)
		nNewLength = file_info.st_size;

	nResult = stat(VARCONF_OLD_FILENAME, &file_info);
	if (nResult != -1)
		nOldLength = file_info.st_size;

	if ((nNewLength < 1024) && (nOldLength > 1024))
    {
        sprintf(buffer, "cp -f %s %s", VARCONF_OLD_FILENAME, VARCONF_FILENAME);
        system(buffer);
    }

	nResult = stat(m_szFileName, &file_info);
	if (nResult != -1)
		nNewLength = file_info.st_size;

	if (nNewLength < 1024)
	{
		fp = fopen(m_szFileName, "wb");
		if (fp != NULL)
		{
			SaveToFile(fp, m_pRootObject, TRUE);
			fflush(fp);
			fclose(fp);
		}
	}

	if (nNewLength > 1024)
	{
		fp = fopen(m_szFileName, "rb");
		if (fp != NULL)
		{
			LoadFromFile(fp, m_pRootObject);
			fclose(fp);
		}
	}
	m_IOLocker.Unlock();
	return VARERR_NOERROR;
}

int CVARService::Save(BOOL bAll)
{
	struct stat file_info;
	FILE	*fp = NULL;
    char buffer[1024];

	if (!m_bGlobalEnableModify)
	{
		m_IOLocker.Lock();
		m_bModified	= FALSE;
		m_IOLocker.Unlock();
		return VARERR_NOERROR;
	}

	m_IOLocker.Lock();
	if (m_bSave)
	{
		if (stat(VARCONF_FILENAME, &file_info) != -1)
		{
			if (file_info.st_size > 1024)
            {
				sprintf(buffer, "cp -f %s %s", VARCONF_FILENAME, VARCONF_OLD_FILENAME);
                system(buffer);
            }
		}

		fp = fopen(m_szFileName, "wb");
		if (fp != NULL)
		{
			SaveToFile(fp, m_pRootObject, bAll);
			fflush(fp);
			fclose(fp);
		}
	}
	m_bModified	= FALSE;
	m_IOLocker.Unlock();
	return (fp == NULL) ? VARERR_IO_ERROR : VARERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Utility Functions
//////////////////////////////////////////////////////////////////////

BOOL CVARService::OnActiveThread()
{
	Save(TRUE);
	return TRUE;
}

BOOL CVARService::OnTimeoutThread()
{
	if (!m_bModified)
		return TRUE;

	return OnActiveThread();
}

//////////////////////////////////////////////////////////////////////
// Utility Functions
//////////////////////////////////////////////////////////////////////

BOOL CVARService::EnumObject(PFNENUMVAROBJECT pfnCallback, void *pParam)
{
	if (!pfnCallback)
		return FALSE;

	if (!m_pRootObject)
		return FALSE;

	NavigateObject(m_pRootObject, pfnCallback, pParam);
	return TRUE;
}

VAROBJECT *CVARService::GetObject(OID3 *oid)
{
	if (m_pRootObject == NULL)
		return NULL;

	return FindObjectByID(m_pRootObject, (BYTE *)oid, 0);
}

VAROBJECT *CVARService::GetObjectByName(const char *pszName)
{
	if (m_pRootObject == NULL)
		return NULL;

	return FindObjectByName(m_pRootObject, pszName);
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

void CVARService::InitObjectValue(VAROBJECT *pList)
{
	int		i;

	for(i=0; pList[i].pszName; i++)
	{
		pList[i].bChanged 	= FALSE;
		pList[i].var.stream.u32 = 0;
		
		switch(pList[i].var.type) {
		  case VARSMI_STRING :
			   pList[i].var.stream.p = (char *)MALLOC(pList[i].var.len+1);
			   memset(pList[i].var.stream.p, 0, pList[i].var.len+1);
			   break;
		  case VARSMI_STREAM :
		  case VARSMI_OPAQUE :
		  case VARSMI_EUI64 :
		  case VARSMI_IPADDR :
		  case VARSMI_TIMESTAMP :
		  case VARSMI_TIMEDATE :	
			   pList[i].var.stream.p = (char *)MALLOC(pList[i].var.len+1);
			   memset(pList[i].var.stream.p, 0, pList[i].var.len+1);
			   break;
		}

		if (pList[i].pfnHandler)
			pList[i].pfnHandler(&pList[i], VARMETHOD_DEFAULT, NULL, NULL, NULL, NULL);

		if (pList[i].pChild != NULL)
			InitObjectValue(pList[i].pChild);
	}
}

void CVARService::FreeObjectValue(VAROBJECT *pList)
{
	int		i;

	for(i=0; pList[i].pszName; i++)
	{
		switch(pList[i].var.type) {
		  case VARSMI_STRING :
		  case VARSMI_STREAM :
		  case VARSMI_OPAQUE :
		  case VARSMI_EUI64 :
		  case VARSMI_IPADDR :
		  case VARSMI_TIMESTAMP :
		  case VARSMI_TIMEDATE :
			   if (pList[i].var.stream.p != NULL)
				   FREE(pList[i].var.stream.p);
			   pList[i].var.stream.p = NULL;
			   break;
		}

        if (pList[i].pChild != NULL)
            FreeObjectValue(pList[i].pChild);
	}
}

VAROBJECT *CVARService::FindObjectByID(VAROBJECT *pList, BYTE *oid, int nIndex)
{
	int		i;

	for(i=0; pList[i].pszName; i++)
	{
		if (oid[nIndex] != pList[i].id)
			continue;

		if ((nIndex == 2) || (oid[nIndex+1] == 0))
			return &pList[i];

		if (pList[i].pChild == NULL)
			return NULL;

		return FindObjectByID(pList[i].pChild, oid, nIndex+1);
	}
	return NULL;
}

VAROBJECT *CVARService::FindObjectByName(VAROBJECT *pList, const char *pszName)
{
	VAROBJECT	*pObject;
	int			i;

	for(i=0; pList[i].pszName; i++)
	{
		if (strcmp(pList[i].pszName, pszName) == 0)
			return &pList[i];

		if (pList[i].pChild != NULL)
		{
			pObject = FindObjectByName(pList[i].pChild, pszName);
			if (pObject != NULL)
				return pObject;
		}
	}
	return NULL;
}

void CVARService::NavigateObject(VAROBJECT *pList, PFNENUMVAROBJECT pfnCallback, void *pParam)
{
	int		i;

	for(i=0; pList[i].pszName; i++)
	{
		pfnCallback(&pList[i], pParam);
		if (pList[i].pChild != NULL)
			NavigateObject(pList[i].pChild, pfnCallback, pParam);
	}
}

const char *GetName(int nType)
{
	switch(nType) {
      case VARSMI_CHAR :		return "CHAR";
      case VARSMI_BOOL :		return "BOOL";
      case VARSMI_BYTE :		return "BYTE";
      case VARSMI_SHORT :		return "SHORT";
      case VARSMI_WORD :		return "WORD";
      case VARSMI_INT :			return "INT";
      case VARSMI_UINT :		return "UINT";
      case VARSMI_OID :			return "OID";
      case VARSMI_STRING :		return "STRING";
      case VARSMI_EUI64 :		return "EUI64";
      case VARSMI_IPADDR :		return "IPADDR";
      case VARSMI_TIMESTAMP :	return "TIMESTAMP";
      case VARSMI_TIMEDATE :	return "TIMEDATE";
      case VARSMI_STREAM :		return "STREAM";
      case VARSMI_OPAQUE :		return "OPAQUE";
    }
	return "<Unknown>";
}

void CVARService::LoadFromFile(FILE *fp, VAROBJECT *pList)
{
	VAROBJECT	*pObject;
	VARITEM		item;
#ifdef DEBUG
	char		szValue[4096], ch[10];
	int			i;
#endif
	int			len, n;	

	while(fread(&item, 10, 1, fp) > 0)
	{
		len = fread(item.stream.str, item.len, 1, fp);

		pObject = GetObject(&item.oid);
		if (pObject != NULL)
		{
#ifdef DEBUG
			*szValue = '\0';
            switch(pObject->var.type) {
              case VARSMI_CHAR :
                   sprintf(szValue, "%c (0x%02X)", item.stream.s8, item.stream.s8);
                   break;
              case VARSMI_BOOL :
                   sprintf(szValue, "%s", item.stream.u8 == 0 ? "FALSE" : "TRUE");
                   break;
              case VARSMI_BYTE :
                   sprintf(szValue, "%u (0x%02X)", item.stream.u8, item.stream.u8);
                   break;
              case VARSMI_SHORT :
                   sprintf(szValue, "%d (0x%04X)", (signed short)item.stream.s16, (signed short)item.stream.s16);
                   break;
              case VARSMI_WORD :
                   sprintf(szValue, "%u (0x%04X)", item.stream.u16, item.stream.u16);
                   break;
              case VARSMI_INT :
                   sprintf(szValue, "%d (0x%08X)", (signed int)item.stream.s32, (signed int)item.stream.s32);
                   break;
              case VARSMI_UINT :
                   sprintf(szValue, "%d (0x%08X)", item.stream.u32, item.stream.u32);
                   break;
              case VARSMI_OID :
                   sprintf(szValue, "%u.%u.%u",
									item.stream.id.id1,
									item.stream.id.id2,
									item.stream.id.id3);
                   break;
              case VARSMI_STRING :
                   memcpy(szValue, item.stream.str, item.len);
                   szValue[item.len] = '\0';
                   break;
              case VARSMI_EUI64 :
              case VARSMI_IPADDR :
              case VARSMI_TIMESTAMP :
              case VARSMI_TIMEDATE :
              case VARSMI_STREAM :
              case VARSMI_OPAQUE :
				   for(i=0; i<item.len; i++)
				   {
					   sprintf(ch, "%02X ", pObject->var.stream.p[i]);
					   strcat(szValue, ch);
				   }
                   szValue[item.len] = '\0';
                   break;
			}

			printf("Reading %0d.%0d.%0d, Length=%0d(%d), Name=%s, Type=%s, Value=%s\r\n", 
					item.oid.id1, item.oid.id2, item.oid.id3,
					item.len, pObject->var.len,
					pObject && pObject->pszName ? pObject->pszName : "<Unknown>",
					pObject ? GetName(pObject->var.type) : "<Unknown>",
					szValue);
#endif
			if (pObject->var.type != item.type)
			{
				XDEBUG("Reading Error %0d.%0d.%0d (%0d) ...\xd\xa", 
					item.oid.id1, item.oid.id2, item.oid.id3,
					item.len);
			}
			else
			{
				switch(pObject->var.type) {
				  case VARSMI_CHAR :
					   pObject->var.stream.s8  = item.stream.s8;
					   break;
				  case VARSMI_BOOL :	
					   pObject->var.stream.u8  = item.stream.u8;
					   break;
				  case VARSMI_BYTE :
					   pObject->var.stream.u8  = item.stream.u8;
					   break;
				  case VARSMI_SHORT :
					   pObject->var.stream.s16 = item.stream.s16;
					   break;
				  case VARSMI_WORD :
					   pObject->var.stream.u16 = item.stream.u16;
					   break;
				  case VARSMI_INT :
					   pObject->var.stream.s32 = item.stream.s32;
					   break;
				  case VARSMI_UINT :
					   pObject->var.stream.u32 = item.stream.u32;
					   break;
				  case VARSMI_OID :
           			   memcpy(&pObject->var.stream.id, item.stream.str, item.len);
					   break;
   			      case VARSMI_STRING :
					   n = item.len > pObject->var.len ? pObject->var.len : item.len;
		               memcpy(pObject->var.stream.p, item.stream.str, n);
 				       pObject->var.stream.p[n] = '\0';
         			   break;    
     			  case VARSMI_EUI64 :
    			  case VARSMI_IPADDR :
     			  case VARSMI_TIMESTAMP :
     			  case VARSMI_TIMEDATE :
      			  case VARSMI_STREAM :
      			  case VARSMI_OPAQUE :
					   n = item.len > pObject->var.len ? pObject->var.len : item.len;
           			   memcpy(pObject->var.stream.p, item.stream.str, n);
					   break;
			    }

				if (pObject->pfnHandler != NULL)
					pObject->pfnHandler(pObject, VARMETHOD_LOAD, NULL, NULL, NULL, NULL);
			}
		}
	}
}

void CVARService::SaveToFile(FILE *fp, VAROBJECT *pList, BOOL bAll)
{
	VARITEM	item;	
	int		i;

	for(i=0; pList[i].pszName; i++)
	{
		if ((pList[i].pChild == NULL) &&
			(bAll || pList[i].bChanged) &&
			(pList[i].var.len > 0))
		{
			memset(&item, 0, sizeof(VARITEM));
			memcpy(&item.oid, &pList[i].var.oid, 3);
			item.type		 	= pList[i].var.type;
			item.len		 	= pList[i].var.len;
			item.lLastModified	= pList[i].lLastModified;
			fwrite(&item, 10, 1, fp);

            if(item.len > 0) {
			    switch(pList[i].var.type) {
			    case VARSMI_CHAR :
			    case VARSMI_BOOL :
			    case VARSMI_BYTE :
			    case VARSMI_SHORT :
			    case VARSMI_WORD :
			    case VARSMI_INT :
			    case VARSMI_UINT :
			    case VARSMI_OID :
           		    fwrite(&pList[i].var.stream.id, item.len, 1, fp);
                    break;
   		        case VARSMI_STRING :
     		    case VARSMI_EUI64 :
    		    case VARSMI_IPADDR :
     		    case VARSMI_TIMESTAMP :
     		    case VARSMI_TIMEDATE :
      		    case VARSMI_STREAM :
      		    case VARSMI_OPAQUE :
           		    fwrite(pList[i].var.stream.p, item.len, 1, fp);
				    break;
		        }
            }
		}

		if (pList[i].pChild != NULL)
			SaveToFile(fp, pList[i].pChild, bAll);
	}
}

BOOL CVARService::Serialize(char *pPath, MIBValue *pList)
{
    FILE *fp;
	MIBValue *m = pList;	

    if(!(fp=fopen(pPath, "w"))) return FALSE;

    while(m) 
    {
        if(fwrite(&m->oid,sizeof(OID3),1,fp)<1) return FALSE;
        if(fwrite(&m->type,sizeof(int),1,fp)<1) return FALSE;
        if(fwrite(&m->len,sizeof(int),1,fp)<1) return FALSE;
        if(m->len) {
			switch(m->type) {
			  case VARSMI_CHAR :
			  case VARSMI_BOOL :
			  case VARSMI_BYTE :
			  case VARSMI_SHORT :
			  case VARSMI_WORD :
			  case VARSMI_INT :
			  case VARSMI_UINT :
			  case VARSMI_OID :
                   if(fwrite(&m->stream.id,m->len,1,fp)<1) return FALSE;
				   break;
   		      case VARSMI_STRING :
     		  case VARSMI_EUI64 :
    		  case VARSMI_IPADDR :
     		  case VARSMI_TIMESTAMP :
     		  case VARSMI_TIMEDATE :
      		  case VARSMI_STREAM :
      		  case VARSMI_OPAQUE :
                   if(fwrite(m->stream.p,m->len,1,fp)<1) return FALSE;
				   break;
		    }
        }
        m = m->pNext;
	}

    fclose(fp);

    return TRUE;
}

BOOL CVARService::Deserialize(char *pPath, MIBValue **pList, int *nCount)
{
    FILE *fp;
    MIBValue *m = NULL;
    MIBValue *h = NULL;
    MIBValue *pm = NULL;
    BOOL    bSucc = TRUE;
    OID3    oid;
    int     nCnt = 0;

    if(!(fp=fopen(pPath, "r"))) return FALSE;
    
    while(fread(&oid, sizeof(OID3), 1, fp)==1) {
        if(m) {
            m->pNext = (MIBValue *) MALLOC(sizeof(MIBValue));
            memset(m->pNext, 0, sizeof(MIBValue));
            pm = m;
            m = m->pNext;
        }else {
            m = (MIBValue *) MALLOC(sizeof(MIBValue));
            memset(m, 0, sizeof(MIBValue));
            h = m;
        }
        memcpy(&m->oid, &oid, sizeof(OID3));
        if(fread(&m->type,sizeof(int),1,fp)<1) { bSucc = FALSE; break; }
        if(fread(&m->len,sizeof(int),1,fp)<1) { bSucc = FALSE; break; }
        if(m->len) {
			switch(m->type) {
			  case VARSMI_CHAR :
			  case VARSMI_BOOL :
			  case VARSMI_BYTE :
			  case VARSMI_SHORT :
			  case VARSMI_WORD :
			  case VARSMI_INT :
			  case VARSMI_UINT :
			  case VARSMI_OID :
                   if(fread(&m->stream.id,m->len,1,fp)<1) {bSucc = FALSE;}
				   break;
   		      case VARSMI_STRING :
     		  case VARSMI_EUI64 :
    		  case VARSMI_IPADDR :
     		  case VARSMI_TIMESTAMP :
     		  case VARSMI_TIMEDATE :
      		  case VARSMI_STREAM :
      		  case VARSMI_OPAQUE :
                   m->stream.p = (char *)MALLOC(m->len+1);
                   if(fread(m->stream.p,m->len,1,fp)<1) {bSucc = FALSE;}
                   m->stream.p[m->len]=0x00;
				   break;
		    }
            if(!bSucc) break;
        }
        nCnt ++;
    }

    *nCount = nCnt;
    if(!bSucc && m) {
		switch(m->type) {
   		  case VARSMI_STRING :
     	  case VARSMI_EUI64 :
    	  case VARSMI_IPADDR :
     	  case VARSMI_TIMESTAMP :
     	  case VARSMI_TIMEDATE :
      	  case VARSMI_STREAM :
      	  case VARSMI_OPAQUE :
                FREE(m->stream.p);
				break;
		}
        FREE(m);
        if(pm) pm->pNext = NULL;
    }

    *pList = h;

    fclose(fp);

    return bSucc;
}
