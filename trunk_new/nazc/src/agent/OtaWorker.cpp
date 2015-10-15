#include "common.h"
#include "codiapi.h"
#include "CommonUtil.h"
#include "AgentService.h"
#include "PackageUtil.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "MeterParserFactory.h"
#include "DebugUtil.h"
#include "AgentLog.h"

#include "OtaWorker.h"
#include "EndDeviceList.h"
#include "Utility.h"

#define BASE_ROM_ADDRESS		0

#define OTA_CMD_VERIFY			0
#define OTA_CMD_INSTALL			1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COtaWorker::COtaWorker()
{
	m_bBusy = FALSE;
	m_pJob = NULL;
	m_pNode = NULL;
	m_pChunk = NULL;
	m_hEvent = NULL;
}

COtaWorker::~COtaWorker()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL COtaWorker::Initialize()
{
	// 쓰레드를 시작 시킨다.
	if (!CTimeoutThread::StartupThread(13))
        return FALSE;
	return TRUE;
}

void COtaWorker::Destroy()
{
	// 쓰레드를 종료한다.
	CTimeoutThread::ShutdownThread();
}

BOOL COtaWorker::IsBusy() 
{
	return m_bBusy;
}

BOOL COtaWorker::SensorOta(OTAENTRY *pJob, OTANODE *pNode, CChunk *pChunk, HANDLE hEvent) 
{
	if (m_bBusy)
		return FALSE;

	m_Locker.Lock();
	m_bBusy = TRUE;
	m_pJob = pJob;
	m_pNode = pNode;
	m_pChunk = pChunk;
	m_hEvent = hEvent;
	m_Locker.Unlock();

	ActiveThread();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Override functions
//////////////////////////////////////////////////////////////////////

BOOL COtaWorker::SingleOta(OTAENTRY *pJob, OTANODE *pNode, CChunk *pChunk, BOOL &bExit)
{
	HANDLE	codi;
	HANDLE	endi;
	ENDI_NODE_INFO	*pNodeInfo;
	time_t	tmNow, tmStart;
	BYTE	*pszBuffer;
	char	*pBinary, *pSeek;
	int		j, nError=0, nSecond;
	int		addr, nRetry, nBytes;
	int		nLength, len, nErrorCount;
	int		nLimitSize;
	WORD	*pValue, shortid, hw, sw, build;
	char	szGUID[17];
	char	szTemp[30];
    BYTE    szVerifyBuf[16];
	UINT	current, start, tick;
	BYTE	hops;
	BOOL	bConnected, bResult;
    CMeterParser * pParser = NULL;
    BOOL    bSubCore = FALSE;

	pszBuffer = (BYTE *)MALLOC(256);
	if (!pszBuffer)
		return FALSE;

	codi = GetCoordinator();
	endi = NULL;
	pNode->bActive = TRUE;
	pNode->nState = STATE_OPEN;
	bConnected = FALSE;
	
	eui64toa(&pNode->id, szGUID);

	if ((pNode->tmEnd != 0) && (pNode->nError != OTAERR_NOERROR))
	{
		// 30초 이내에 재시도 된경우 나머지 시간동안 대기한다.
		for(;!bExit && !pNode->bExitPending;)
		{
		    time(&tmNow);
			nSecond = abs(tmNow-pNode->tmEnd);
			if (nSecond > 30)
				break;
			
			// 1초간 대기한다.
			XDEBUG("SingleOta: Waitting (%0d remain)\r\n", 30-nSecond);
			usleep(1000000);
		}
	}

	time(&pNode->tmStart);
	pNode->nError = OTAERR_NOERROR;
	for(nRetry=0; (pNode->nState != STATE_DONE);)
	{
		if (bExit || pNode->bExitPending)
		{
			if (bConnected) endiDisconnect(endi);
			if (endi != NULL) endiClose(endi);
			break;
		}

		switch(pNode->nState) {
		  case STATE_OPEN :
			   endi = NULL;
			   bConnected = FALSE;

			   if ((pNode->nOtaStep == OTA_STEP_SCAN) || ((pNode->nOtaMask & OTA_STEP_INSTALL) != 0))
			   {
				   // SCAN만 선택한 경우
				   pNode->nState = STATE_SCAN;
				   break;
			   }

			   XDEBUG("OTA STATE OPEN [%s]\r\n", szGUID);
			   nError = endiOpen(codi, &endi, &pNode->id);
			   if (nError != CODIERR_NOERROR)
			   {
				   pNode->nError = GetOtaError(nError, OTAERR_OPEN_ERROR);
				   pNode->nState = STATE_RETRY;
				   break;
			   }

			   endiSetOption(endi, ENDIOPT_SEQUENCE, 0);
			   bConnected = TRUE;
			   pNode->nError = OTAERR_NOERROR;
			   pNode->nState = STATE_WAIT_CONNECT;
			   break;

		  case STATE_WAIT_CONNECT :
			   XDEBUG("OTA STATE CONNECT [%s]\r\n", szGUID);
			   nError = sensorConnect(endi, pNode->nConnectionTimeout);
			   if (nError != CODIERR_NOERROR)
			   {
				   pNode->nError = GetOtaError(nError, OTAERR_CONNECT_ERROR);
				   pNode->nState = STATE_RETRY;
				   break;
			   }

			   pNode->nError = OTAERR_NOERROR;
			   pNode->nState = STATE_SENSOR_INVENTORY;
			   break;

		  case STATE_SENSOR_INVENTORY :
			   if ((pNode->nOtaMask & OTA_STEP_INVENTORY) != 0)
			   {
				   pNode->nState = STATE_SELECT_FIRMWARE;
				   break;
			   }

			   XDEBUG("OTA STATE INVENTORY [%s]\r\n", szGUID);
	           pJob->nOtaCurrentStep |= OTA_STEP_INVENTORY;
               m_pEndDeviceList->SetInstallState(&pNode->id, (BYTE)pJob->nOtaCurrentStep);
               m_pEndDeviceList->VerifyEndDevice(&pNode->id);
			   for(j=0; (j<3) && !bExit && !pNode->bExitPending; j++)
			   {
				   addr = ENDI_ROMAP_NODE_KIND;
				   len = sizeof(ENDI_NODE_INFO);

				   nError = endiReadRom(endi, addr, len, (BYTE *)pszBuffer, &nBytes, pNode->nReplyTimeout);
				   if (nError == CODIERR_NOERROR)
					   break;
			   }

			   if (nError != CODIERR_NOERROR)
			   {
				   pNode->nError = GetOtaError(nError, OTAERR_INVENTORY_SCAN_ERROR);
				   pNode->nState = STATE_RETRY;
				   break;
			   }

			   pNodeInfo = (ENDI_NODE_INFO *)pszBuffer;
			   memset(pNode->szModel, 0, sizeof(pNode->szModel));
               stripSafeCpy(pNode->szModel, sizeof(pNode->szModel), pNodeInfo->NODE_KIND);
			   pNode->hw	= pNodeInfo->HARDWARE_VERSION;
			   pNode->sw	= pNodeInfo->FIRMWARE_VERSION;
			   pNode->build = pNodeInfo->FIRMWARE_BUILD;
               if(pJob->nDestType==1)
               {
                    if(strcasecmp(pJob->szDestParserName, "SubSx") == 0 )
                    {
#if 0
                        /** TODO : SX Meter의 버전 검사 및 설정 부분은 다시 작성한다 */
                        strcpy(szVerName, pJob->szBinURL);  // jiwook add 
                        pos = strrchr( szVerName , '_' );     // jiwook add
                        strcpy(szVerTemp, pos +1 );        // jiwook add
                        strcpy(szVerName, szVerTemp ); 
                        pos = strtok(szVerName , "."); 	
#endif
                    }
                    else 
                    {
                        for(j=0; (j<3) && !bExit && !pNode->bExitPending; j++)
                        {
					        addr = ENDI_ROMAP_SUB_FW_VERSION;
					        len  = 2;
        
                            nError = endiReadRom(endi, addr, len, (BYTE *)pszBuffer, &nBytes, pNode->nReplyTimeout);
                            if (nError == CODIERR_NOERROR)
                                    break;
                        }                  
			            pNode->sw	= pszBuffer[0];
			            pNode->build = pszBuffer[1];
                        XDEBUG("OTA STATE SUB CPU VERSION CHECK [%s]sw[%x]bu[%x]\r\n", szGUID, pNode->sw, pNode->build);
                    }
               }
			   pNode->nOtaMask |= OTA_STEP_INVENTORY;	
			   pNode->nError = OTAERR_NOERROR;
			   pNode->nState = STATE_SELECT_FIRMWARE;
			   break;

		  case STATE_SELECT_FIRMWARE :
			   XDEBUG("OTA STATE CHECK MODEL & VERSION [%s] [%s]\r\n", szGUID, pJob->szDestParserName);
               /** TODO : Model 명을 검사할 때 Parser에 따라 다른 Model 명이 올 수 있다.
                 * Sensor에 대한 정확한 모델명을 검사하는게 맞는건지 아니면 Parser 명을 제공하는게
                 * 맞는건지 검토해 봐야 한다.
                 */
               pParser = m_pMeterParserFactory->SelectParser(pNode->szModel);
               if(pParser && pJob->nDestType == 1) // SubCore
               {
                   pParser = pParser->GetSubCoreParser();
               }
			   if (!pParser || (strncmp(pParser->GetName(), pJob->szDestParserName, strlen(pJob->szDestParserName)) != 0))
			   {
			 	   // 센서의 모델명이 다른경우 실패 시키고 더이상 진행하지 않토록 한다.
				   bResult = FALSE;
			   } 
               else 
               {
			       bResult = TRUE;
                   sw = ((pNode->sw & 0xf0) << 4) + (pNode->sw & 0x0f);
                   build = (((pNode->build & 0xf0) >> 4) * 10) + (pNode->build & 0x0f);
			       switch(pJob->nInstallType) {
			         case OTA_INSTALL_AUTO :
					      // 자동인 경우 같은 버젼은 설치 안함. 
			   		      if ((pJob->nNewSw == sw) && (pJob->nNewBuild == build))
				          {
						      pNode->nError = OTAERR_VERSION_ERROR;
				   		      pNode->bSkip = TRUE;
				   		      bResult = FALSE;
					      }
					      break;
	    
				     case OTA_INSTALL_MATCH :
					      // MATCH의 경우 구 버젼이 현재버젼과 같지 않으면 설치 안함.	
					      if ((pJob->nOldSw != sw) || (pJob->nOldBuild != build))
					      {
						      pNode->nError = OTAERR_VERSION_ERROR;
				   		      pNode->bSkip = TRUE;
				   		      bResult = FALSE;
					      }
					      break;

 			         case OTA_INSTALL_REINSTALL :
 			         case OTA_INSTALL_FORCE :
					      // REINSTALL의 경우 무조건 설치함.
					      break;
			       }	
               }
	
			   if (!bResult)
			   {
			       if (bConnected) endiDisconnect(endi);
			       if (endi != NULL) endiClose(endi);

				   bConnected = FALSE;
				   endi = NULL;
			       pNode->nState = STATE_OK;
				   break;
			   }

			   pNode->nError = OTAERR_NOERROR;
			   pNode->nState = STATE_SEND_FIRMWARE;
			   break;

		  case STATE_SEND_FIRMWARE :
			   if (((pNode->nOtaStep & OTA_STEP_SEND) == 0) || ((pNode->nOtaMask & OTA_STEP_SEND) != 0))
			   {
				   pNode->nState = STATE_VERIFY;
				   break;
			   }

			   XDEBUG("OTA SEND FIRMWARE [%s]\r\n", szGUID);
			   bResult = FALSE;
			   nErrorCount = 0;
			   pBinary = pChunk->GetBuffer();
			   current = pNode->nElapse;
			   start = (UINT)uptime();
			   time(&tmStart);

	           pJob->nOtaCurrentStep |= OTA_STEP_SEND;
               m_pEndDeviceList->SetInstallState(&pNode->id, (BYTE)pJob->nOtaCurrentStep);
               m_pEndDeviceList->VerifyEndDevice(&pNode->id);

			   for(;!bExit && !pNode->bExitPending;)
			   {
				   addr = BASE_ROM_ADDRESS + pNode->nCurrent;
				   pSeek = (pBinary + pNode->nCurrent);

				   m_pEndDeviceList->QueryRouteRecord(&pNode->id, &shortid, &hops, NULL);
				   pNode->nHops = hops;
				   nLimitSize = MAX_OTA_BYTES - hops * sizeof(WORD);
				   nLength = MIN((pNode->nTotal - pNode->nCurrent), nLimitSize);
				   
				   XDEBUG("endiWriteLongRom: nTotal=%0d, nCurrent=%0d, nLength=%0d, HOP=%0d\r\n",
							pNode->nTotal, pNode->nCurrent, nLength, hops);
				   nError = endiWriteLongRom(endi, addr, (BYTE *)pSeek, nLength,
													     (BYTE *)pszBuffer, &nBytes, pNode->nReplyTimeout);
				   if ((nError == CODIERR_NOERROR) && (nBytes == nLength) &&
					   (memcmp(pSeek, pszBuffer, nLength) == 0))
				   {
					   pNode->nCurrent += nLength;
					   nErrorCount = 0;
				   }
				   else
				   {
					   nErrorCount++;
					   XDEBUG("\033[1;40;31mendiWriteLongRom Error(%0d): nError=%0d, nLength=%0d, nBytes=%0d, Compare=%0d\033[0m\r\n",
								nErrorCount, nError, nLength, nBytes, memcmp(pSeek, pszBuffer, nLength));
				   }

				   time(&tmNow);
			   	   pNode->nElapse = current + ((UINT)tmNow-(UINT)tmStart);
				   if (nErrorCount > 5)
				   {
				   	   pNode->nError = OTAERR_SEND_ERROR;
					   break;
				   }
	
				   if (pNode->nCurrent >= pNode->nTotal)
				   {
			   		   pNode->nOtaMask |= OTA_STEP_SEND;	
					   bResult = TRUE;
					   break;
				   }
			   }

			   tick = (UINT)uptime();
			   pNode->nElapse = current + (tick-start);
			   pNode->nError = OTAERR_NOERROR;
			   pNode->nState = bResult ? STATE_VERIFY : STATE_RETRY;
			   break;

		  case STATE_VERIFY :
			   if (((pNode->nOtaStep & OTA_STEP_INSTALL) != 0) && ((pNode->nOtaMask & OTA_STEP_VERIFY) == 0))
			   {
				   // 설치는 하면서 VERIFY를 하지 않은 경우 무조건 Verify를 시킨다.
				   pNode->nOtaStep |= OTA_STEP_VERIFY;
			   }
			   else if (((pNode->nOtaStep & OTA_STEP_VERIFY) == 0) || ((pNode->nOtaMask & OTA_STEP_VERIFY) != 0))
			   {
				   pNode->nState = STATE_INSTALL;
				   break;
			   }
                // crc len add
               UINT htolFWlen;
               WORD htosFWCRC;

               memset(szVerifyBuf, 0, sizeof(szVerifyBuf));
               htolFWlen = BigToHostInt(pJob->nFWLen);
               htosFWCRC = BigToHostShort(pJob->nFWCRC);
               memcpy(szVerifyBuf, &htolFWlen, 4);
               memcpy(szVerifyBuf+4, &htosFWCRC, 2);
               if(pJob->nDestType == 1)
               {
                   /** Sub Core : Issue #89 */
                   szVerifyBuf[6] = 0x01;
               }

			   XDEBUG("OTA STATE VERIFY [%s], %02X, %04X\r\n", szGUID, pJob->nFWCRC, pJob->nFWLen);
               XDUMP((char *)szVerifyBuf, 7);

	           pJob->nOtaCurrentStep |= OTA_STEP_VERIFY;
               m_pEndDeviceList->SetInstallState(&pNode->id, (BYTE)pJob->nOtaCurrentStep);
               m_pEndDeviceList->VerifyEndDevice(&pNode->id);
			   bResult = FALSE;

			   for(j=0; (j<3) && !pNode->bExitPending && !bExit; j++)
			   {
				   //nError = endiCommand(endi, OTA_CMD_VERIFY, (BYTE *)const_cast<char *>(""), 0, (BYTE *)pszBuffer, &nBytes, 15000);
				   nError = endiCommandWrite(endi, OTA_CMD_VERIFY, szVerifyBuf, 7, (BYTE *)pszBuffer, &nBytes, 15000);
				   if (nError != CODIERR_NOERROR)
					   continue;
	
				   if (nBytes == 2)
				   {
				   	   pValue = (WORD *)pszBuffer;
					   if ((pJob->nDestType == 0 && *pValue != 0) || (pJob->nDestType != 0 && *pValue == 0))
					   {
				   		   bResult = TRUE;
			   		       pNode->nOtaMask |= OTA_STEP_VERIFY;	
			               XDEBUG("OTA STATE VERIFY [%s] RESULT SUCC\r\n", szGUID);
				   		   break;
					   }
					   else
					   {
						   // Verify에 실패한 경우, 처음부터 다시 시도한다.
						   pNode->nOtaMask = 0;
						   pNode->nCurrent = 0;
						   pNode->nElapse = 0;
			               XDEBUG("OTA STATE VERIFY [%s] RESULT FAIL\r\n", szGUID);
						   break;
					   }
				   }
			   }

			   pNode->nError = OTAERR_NOERROR;
			   pNode->nState = bResult ? STATE_INSTALL : STATE_RETRY;
			   break;

	      case STATE_INSTALL :
			   if (((pNode->nOtaStep & OTA_STEP_INSTALL) == 0) || ((pNode->nOtaMask & OTA_STEP_INSTALL) != 0))
			   {
			   	   endiDisconnect(endi);
			   	   endiClose(endi);

			   	   bConnected = FALSE;
			   	   endi = NULL;

				   pNode->nState = STATE_SCAN;
				   break;
			   }

	           pJob->nOtaCurrentStep |= OTA_STEP_INSTALL;
               m_pEndDeviceList->SetInstallState(&pNode->id, (BYTE)pJob->nOtaCurrentStep);
               m_pEndDeviceList->VerifyEndDevice(&pNode->id);

			   XDEBUG("OTA STATE INSTALL [%s]\r\n", szGUID);
			   bResult = FALSE;
			   for(j=0; (j<3) && !pNode->bExitPending && !bExit; j++)
			   {
				   //nError = endiCommand(endi, OTA_CMD_INSTALL, (BYTE *)const_cast<char *>(""), 0, (BYTE *)pszBuffer, &nBytes, 15000);
				   nError = endiCommandWrite(endi, OTA_CMD_INSTALL, &pJob->nDestType, 1, (BYTE *)pszBuffer, &nBytes, 15000);
				   if (nError != CODIERR_NOERROR)
					   continue;
	
				   bResult = TRUE;
				   break;
			   }

			   if (!bResult)
			   {
			   	   pNode->nState = STATE_RETRY;
				   break;
			   }
			
			   endiDisconnect(endi);
			   endiClose(endi);

			   bConnected = FALSE;
			   endi = NULL;

			   pNode->nOtaMask |= OTA_STEP_INSTALL;	
			   pNode->nError = OTAERR_NOERROR;
			   pNode->nState = STATE_SCAN;
			   break;

		  case STATE_SCAN :
			   if ((pNode->nOtaStep & OTA_STEP_SCAN) == 0)
			   {
				   pNode->nState = STATE_OK;
				   break;
			   }

			   XDEBUG("OTA STATE SCAN [%s]\r\n", szGUID);
	           pJob->nOtaCurrentStep |= OTA_STEP_SCAN;
               m_pEndDeviceList->SetInstallState(&pNode->id, (BYTE)pJob->nOtaCurrentStep);
               m_pEndDeviceList->VerifyEndDevice(&pNode->id);
			   bResult = FALSE;

               if(pJob->nDestType==1)
               {
                   /** SX Meter FW OTA의 경우 Upgrade 시 많은 시간이 필요하기 때문에 Scan을 시도하지 않는다 */
                    if(strcasecmp(pJob->szDestParserName, "SubSx") == 0 )
                    {
			            pNode->nOtaMask |= OTA_STEP_SCAN;	
			            pNode->nError = OTAERR_NOERROR;
			            pNode->nState = STATE_OK;
			            bResult = TRUE;
                        break;
                    }
               }

			   for(j=0; (j<3) && !bExit && !pNode->bExitPending; j++)
			   {
                   pParser = NULL;
                   bSubCore = FALSE;

			   	   nError = endiOpen(codi, &endi, &pNode->id);
				   if (nError == CODIERR_NOERROR)
				   {
					   nError = sensorConnect(endi, pNode->nConnectionTimeout);
					   if (nError == CODIERR_NOERROR)
					   {
						   addr = ENDI_ROMAP_NODE_KIND;
						   len = sizeof(ENDI_NODE_INFO);

						   nError = endiReadRom(endi, addr, len, (BYTE *)pszBuffer, &nBytes, pNode->nReplyTimeout);
						   if (nError == CODIERR_NOERROR)
						   {
			   				   pNodeInfo = (ENDI_NODE_INFO *)pszBuffer;
			   				   memset(pNode->szModel, 0, sizeof(pNode->szModel));
                               stripSafeCpy(pNode->szModel, sizeof(pNode->szModel), pNodeInfo->NODE_KIND);
			   				   pNode->hwNew		= pNodeInfo->HARDWARE_VERSION;
			   				   pNode->swNew		= pNodeInfo->FIRMWARE_VERSION;
			   				   pNode->buildNew 	= pNodeInfo->FIRMWARE_BUILD;

			   				   XDEBUG("-------------- SCAN COMPLETE ------------\r\n");
							   hw = ((pNode->hwNew & 0xf0) << 4) + (pNode->hwNew & 0x0f);
							   sw = ((pNode->swNew & 0xf0) << 4) + (pNode->swNew & 0x0f);
							   build = (((pNode->buildNew & 0xf0) >> 4) * 10) + (pNode->buildNew & 0x0f);
                               /** Model을 변경하지 않는다 **/
							   m_pEndDeviceList->UpdateSensorInventory(&pNode->id, NULL, PARSER_TYPE_NOT_SET, NULL, hw, sw, build);
                               pParser = m_pMeterParserFactory->SelectParser(pNode->szModel);
                               if(pParser && (pParser->HasSubCore() || pParser->IsSubCore())) {
                                   bSubCore = TRUE;
                               }else {
			   				       pNode->nOtaMask |= OTA_STEP_SCAN;	
							       bResult = TRUE;
                               }
						   }

                           if(bSubCore) {
						        addr = ENDI_ROMAP_SUB_FW_VERSION;
						        len = 2;

						        nError = endiReadRom(endi, addr, len, (BYTE *)pszBuffer, &nBytes, pNode->nReplyTimeout);
						        if (nError == CODIERR_NOERROR)
						        {
                                    m_pEndDeviceList->UpdateSubInventory(&pNode->id,NULL,
                                        (((WORD) (pszBuffer[0] & 0xf0)) << 4) + (pszBuffer[0] & 0x0f),
                                        (((WORD) (pszBuffer[1] >> 4)) * 10) + (pszBuffer[1] & 0x0f));
			   				        pNode->nOtaMask |= OTA_STEP_SCAN;	
							        bResult = TRUE;
						        }
                           }

			   			   endiDisconnect(endi);
					   }
			   		   endiClose(endi);
					}
                    if(bResult) break;

				    // SCAN에 한번 실패하면 3초후에 다시 시도한다.
				    usleep(pNode->nReplyTimeout);
			   }

			   if (!bResult)
			   {
			   	   endi = NULL;
			   	   pNode->nState = STATE_RETRY;
				   break;
			   }

			   pNode->nError = OTAERR_NOERROR;
			   pNode->nState = STATE_OK;
			   break;

		  case STATE_RETRY :
			   XDEBUG("OTA STATE RETRY [%s] [%d]\r\n", szGUID, nRetry + 1);
			   if (bConnected) endiDisconnect(endi);
			   if (endi != NULL) endiClose(endi);

			   bConnected = FALSE;
			   endi = NULL;
			
			   // 1초 대기후 다시 시도한다.
			   nRetry++;
			   if (nRetry > 2)
			   {
			   	   pNode->nState = STATE_DONE;
				   break;
			   }
			   usleep(1000000);
			   pNode->nState = STATE_OPEN;
			   break;

		  case STATE_OK :
			   XDEBUG("OTA STATE OK [%s]\r\n", szGUID);
			   pNode->bDone = TRUE;

			   if (pJob->nOtaStep == pNode->nOtaMask)
			   {
				   GetTimeString(szTemp, pNode->nElapse);
				   UPGRADE_LOG("[%s] %s(HW=%0d.%0d, SW=%0d.%0d, BUILD=%0d, HOP=%0d) OK (Elapse=%s)\r\n",
							szGUID, pNode->szModel,
							pNode->hw >> 4, pNode->hw & 0x0f,
							pNode->sw >> 4, pNode->sw & 0x0f,
							((pNode->build >> 4) * 10) + (pNode->build & 0x0f),
							pNode->nHops, szTemp);
				   otaResultEvent(pJob->szTriggerId, &pNode->id, 0, pNode->nOtaMask, pNode->nError);
			   }
			   pNode->nState = STATE_DONE;
			   break;
		}
	}

	time(&pNode->tmEnd);
	pNode->bActive = FALSE;
	if(pszBuffer) FREE(pszBuffer);
	return TRUE;
}

BOOL COtaWorker::OnActiveThread()
{
	if ((m_pJob == NULL) || (m_pNode == NULL))
		return TRUE;

	SingleOta(m_pJob, m_pNode, m_pChunk, m_pJob->bExitPending);

	m_Locker.Lock();
	m_pJob = NULL;
	m_pNode = NULL;
	m_pChunk = NULL;
	SetEvent(m_hEvent);
	m_hEvent = NULL;
	m_bBusy = FALSE;
	m_Locker.Unlock();
	return TRUE;
}

BOOL COtaWorker::OnTimeoutThread()
{
	return OnActiveThread();
}

