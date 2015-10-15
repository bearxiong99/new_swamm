#include "common.h"
#include "codiapi.h"
#include "AgentService.h"
#include "MeterDeviceList.h"
#include "EndDeviceList.h"
#include "CommonUtil.h"
#include "Utility.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Data definitions
//////////////////////////////////////////////////////////////////////

CMeterDeviceList   *m_pMeterDeviceList = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeterDeviceList::CMeterDeviceList()
{
	m_pMeterDeviceList = this;
}

CMeterDeviceList::~CMeterDeviceList()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CMeterDeviceList::Initialize()
{
	LoadMeterDevice("/app/member");
	return TRUE;
}

void CMeterDeviceList::Destroy()
{
    m_MeterLocker.Lock();
	RemoveAll();
	m_MeterLocker.Unlock();
}


//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CMeterDeviceList::Add(const EUI64 *id, BYTE port, char *serial, BYTE *menu, 
        BYTE gener, BYTE media, BYTE Access, BYTE status)
{
	METERDEVICENODE	*pNode;
    GEUI64 gid;

    memcpy(&gid.gid, id, sizeof(EUI64));
    gid.gport = port;
    if(port == 0) return FALSE;

	m_MeterLocker.Lock();
	pNode = FindNode(&gid);
	if (pNode == NULL)
	{
		pNode = (METERDEVICENODE *)MALLOC(sizeof(METERDEVICENODE));
		if (pNode != NULL)
		{
   			memset(pNode, 0, sizeof(METERDEVICENODE));
			memcpy(&pNode->gid, &gid, sizeof(GEUI64));
			memcpy(&pNode->szSerial, serial, sizeof(pNode->szSerial));
			memcpy(&pNode->szMenufa, menu, sizeof(pNode->szMenufa));
			pNode->szGener  	= gener;
			pNode->szMedia  	= media;
            pNode->szCntAccess  = Access;
            pNode->szstatus     = status;
            time(&pNode->tmMetering);
			pNode->nPosition = (int)m_List.AddTail(pNode);
		}
	}
	else        //Setup time set
	{
   			memset(pNode, 0, sizeof(METERDEVICENODE));
			memcpy(&pNode->gid, &gid, sizeof(GEUI64));
			memcpy(&pNode->szSerial, serial, sizeof(pNode->szSerial));
			memcpy(&pNode->szMenufa, menu, sizeof(pNode->szMenufa));
			pNode->szGener  	= gener;
			pNode->szMedia  	= media;
            pNode->szCntAccess  = Access;
            pNode->szstatus     = status;
            time(&pNode->tmMetering);
	}
	m_MeterLocker.Unlock();
    SaveMeterDevice(pNode);
	return pNode != NULL ? TRUE : FALSE;
}

BOOL CMeterDeviceList::Remove(const EUI64 *id, BYTE port)
{
	METERDEVICENODE	*pNode;
    GEUI64 gid;
    char        szCommand[128];
    char        szGUID[20];

    memcpy(&gid.gid, id, sizeof(EUI64));
    gid.gport = port;
    if(port == 0) return FALSE;

    m_MeterLocker.Lock();
	pNode = FindNode(&gid);
	if (pNode != NULL)
	{
		m_List.RemoveAt((POSITION)pNode->nPosition);
		FREE(pNode);	
		eui64toa(id, szGUID);
		sprintf(szCommand, "/app/member/%s_%d.slr", szGUID, port);
		unlink(szCommand);
	}
	m_MeterLocker.Unlock();
	return pNode != NULL ? TRUE : FALSE;
}

BOOL CMeterDeviceList::Remove(const EUI64 *id)
{
	METERDEVICENODE	*pNode;
    char        szCommand[128];
    char        szGUID[20];
    
    eui64toa(id, szGUID);

    m_MeterLocker.Lock();
    while((pNode = FindNode(id)) != NULL)
    {
        m_List.RemoveAt((POSITION)pNode->nPosition);
        sprintf(szCommand, "/app/member/%s_%d.slr", szGUID, pNode->gid.gport);
        unlink(szCommand);
        FREE(pNode);	
    }
	m_MeterLocker.Unlock();
	return TRUE;
}


METERDEVICENODE * CMeterDeviceList::GetMeter(const EUI64 *id, BYTE port)
{
	METERDEVICENODE	*pNode;
    GEUI64 gid;

    memcpy(&gid.gid, id, sizeof(EUI64));
    gid.gport = port;
    if(port == 0) return FALSE;

    m_MeterLocker.Lock();
	pNode = FindNode(&gid);
	m_MeterLocker.Unlock();
	return pNode;
}

METERDEVICENODE * CMeterDeviceList::GetMeter(const EUI64 *id, METERDEVICENODE * prev)
{
	METERDEVICENODE	*pNode;

    m_MeterLocker.Lock();
	pNode = FindNode(id, prev);
	m_MeterLocker.Unlock();
	return pNode;
}

void CMeterDeviceList::RemoveAll()
{
	METERDEVICENODE	*pItem, *pDelete;
	POSITION		pos;

	m_MeterLocker.Lock();
	pItem = m_List.GetFirst(pos);
	while(pItem)
	{
		pDelete = pItem;
		pItem = m_List.GetNext(pos);
		FREE(pDelete);
	}
	m_List.RemoveAll();
	m_MeterLocker.Unlock();
}

BOOL CMeterDeviceList::DeleteMeterDeviceAll()
{
	m_MeterLocker.Lock();


	RemoveAll();
	m_List.RemoveAll();

	m_MeterLocker.Unlock();

	system("rm -rf /app/member/*");
	system("rm -rf /app/data/*");
    /*-- 너무 많은 파일이 있을 경우 delete error가 날것을 염려해서 Directory를 삭제한다 --*/
	system("rm -rf /app/member");
	system("rm -rf /app/data");

	mkdir("/app/member", 0777);
	mkdir("/app/data",  0777);
	return TRUE;
}

BOOL CMeterDeviceList::SaveMeterDevice(METERDEVICENODE *pMeterDevice)
{
	FILE	*fp;
	char	szFileName[128];
	char	szGUID[20];

	eui64toa(&pMeterDevice->gid.gid, szGUID);	
	sprintf(szFileName, "/app/member/%s_%d.slr", szGUID,pMeterDevice->gid.gport);

	fp = fopen(szFileName, "wb");
	if (fp == NULL){
        XDEBUG("fp=NULL\xd\xa");
		return FALSE;
    }

	fwrite(pMeterDevice, sizeof(METERDEVICENODE), 1, fp);

	fflush(fp);
	fclose(fp);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

void CMeterDeviceList::SetRemove(const EUI64 *id)
{
	METERDEVICENODE	*pNode;
	POSITION		pos=INITIAL_POSITION;

    if(!id) return;

    m_MeterLocker.Lock();
	pNode = m_List.GetFirst(pos);
	while(pNode)
	{
		if (memcmp(&pNode->gid.gid, id, sizeof(EUI64)) == 0)
        {
            pNode->bRemove = TRUE;
        }
		pNode = m_List.GetNext(pos);
	}
	m_MeterLocker.Unlock();
}

void CMeterDeviceList::ResetRemove(const EUI64 *id, BYTE nPort)
{
	METERDEVICENODE	*pNode;
	POSITION		pos=INITIAL_POSITION;

    if(!id) return;

    m_MeterLocker.Lock();
	pNode = m_List.GetFirst(pos);
	while(pNode)
	{
		if (!memcmp(&pNode->gid.gid, id, sizeof(EUI64)) && (pNode->gid.gport == nPort))
        {
            pNode->bRemove = FALSE;
            break;
        }
		pNode = m_List.GetNext(pos);
	}
	m_MeterLocker.Unlock();
}

void CMeterDeviceList::Clear(const EUI64 *id)
{
	METERDEVICENODE	*pNode;
	POSITION		pos=INITIAL_POSITION;
    char        szCommand[128];
    char        szGUID[20];

    if(!id) return;

    eui64toa(id, szGUID);

    m_MeterLocker.Lock();
	pNode = m_List.GetFirst(pos);
	while(pNode)
	{
		if (!memcmp(&pNode->gid.gid, id, sizeof(EUI64)) && pNode->bRemove)
        {
            m_List.RemoveAt((POSITION)pNode->nPosition);
            sprintf(szCommand, "/app/member/%s_%d.slr", szGUID, pNode->gid.gport);
            unlink(szCommand);
            FREE(pNode);	
        }
		pNode = m_List.GetNext(pos);
	}
	m_MeterLocker.Unlock();
}


METERDEVICENODE *CMeterDeviceList::FindNode(const GEUI64 *gid)
{
	METERDEVICENODE	*pNode;
	POSITION		pos=INITIAL_POSITION;
	
	pNode = m_List.GetFirst(pos);
	while(pNode)
	{
		if (memcmp(&pNode->gid, gid, sizeof(GEUI64)) == 0)
			return pNode;
		pNode = m_List.GetNext(pos);
	}
	return pNode;
}

METERDEVICENODE *CMeterDeviceList::FindNode(const EUI64 *id)
{
    return FindNode(id, NULL);
}

METERDEVICENODE *CMeterDeviceList::FindNode(const EUI64 *id, METERDEVICENODE * prev)
{
	METERDEVICENODE	*pNode;
	POSITION		pos=INITIAL_POSITION;

    if(!prev) {
        pNode = m_List.GetFirst(pos);
    } else {
        pos = m_List.Get(prev);
        pNode = m_List.GetNext(pos);
    }

	while(pNode)
	{
		if (memcmp(&pNode->gid.gid, id, sizeof(EUI64)) == 0)
			return pNode;
		pNode = m_List.GetNext(pos);
	}
	return pNode;
}

/*
METERDEVICENODE *CMeterDeviceList::FindSerial(const EUI64 *id, int address)
{
	METERDEVICENODE	*pMeterDevice;
	POSITION		pos=INITIAL_POSITION;

	pMeterDevice = m_List.GetFirst(pos);
	while(pMeterDevice)
	{
       if((memcmp(&pMeterDevice->gid.gid,id, sizeof(EUI64))==0)&&(pMeterDevice->szMasterBank==address))
       {
            XDEBUG("BINGO\xd\xa");
            return pMeterDevice;
        }
		pMeterDevice = m_List.GetNext(pos);
	}
    return NULL;
}
*/

BOOL CMeterDeviceList::LoadMeterDevice(const char *pszPath)
{
	METERDEVICENODE	*pMeterDevice, meter;
    DIR			*dir_fdesc;
    dirent		*dp;
	int			fd, n, sn;
    BOOL        bInvalidStructure = FALSE;
	char		*p, szFileName[128];

    dir_fdesc = opendir(pszPath);
    if (!dir_fdesc)
        return FALSE;

    sn = sizeof(METERDEVICENODE);

    m_MeterLocker.Lock();
    for(;(dp=readdir(dir_fdesc));)
    {
        p = strrchr(dp->d_name, '.');
        if ((p == NULL) || (strcmp(p, ".slr") != 0))
            continue;

        sprintf(szFileName, "%s/%s", pszPath, dp->d_name);

        if(bInvalidStructure) {
            XDEBUG("Delete %s (invalid structure)\r\n", dp->d_name);
            remove(szFileName);
        }

        fd = open(szFileName, O_RDONLY);
        if (fd > 0)
        {
			memset(&meter, 0, sn);
			n = read(fd, &meter, sn);
			close(fd);

			if (n == sn)
			{
				pMeterDevice = (METERDEVICENODE *)MALLOC(sizeof(METERDEVICENODE));
				if (pMeterDevice != NULL)
				{
					memset(pMeterDevice, 0, sizeof(METERDEVICENODE));
					memcpy(pMeterDevice, &meter, sizeof(METERDEVICENODE));
                    pMeterDevice->bRemove = FALSE;
                    pMeterDevice->nPosition  = (int)m_List.AddTail(pMeterDevice);
				}
			}
			else
			{
                bInvalidStructure = TRUE;

	            XDEBUG("EndDeviceList: Invalid Structure Size\r\n");
                break;
			}
		}
	}
	closedir(dir_fdesc);
	m_MeterLocker.Unlock();

    if(bInvalidStructure) {
	    XDEBUG("Delete All Members - Invalid Structure\r\n");
        DeleteMeterDeviceAll();
    }

	return TRUE;
}



int CMeterDeviceList::EnumMeterDevice(PFNENUMMETERDEVICE pfnCallback, void *pParam, void *pParam1, void *pParam2)
{
	METERDEVICENODE	*pMeterDevice;
	POSITION	pos;
	int			nCount=0;

	m_MeterLocker.Lock();
	pMeterDevice = m_List.GetFirst(pos);
	while(pMeterDevice)
	{
		if(pfnCallback(pMeterDevice, pParam, pParam1, pParam2)) nCount++;
		pMeterDevice = m_List.GetNext(pos);
	}
	m_MeterLocker.Unlock();
	return nCount;
}

