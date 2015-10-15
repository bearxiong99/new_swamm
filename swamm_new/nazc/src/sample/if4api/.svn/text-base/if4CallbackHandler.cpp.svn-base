#include <stdio.h>

#include "typedef.h"
#include "if4api.h"
#include "CommonUtil.h"
#include "if4frame.h"

// 검침데이터 또는 기타 파일을 전송한 경우 (내부 형식은 관계 없음)
void if4OnDataFile(char *pszAddress, UINT nSourceId, BYTE nType, char *pszName, BYTE *pData, int nLength)
{
}

/** Metering Data Format.
  *
  * AiMiR System Metering Data Format.
  */
typedef struct  _tagMETERLPENTRY
{
    EUI64      	mlpId; 
    char        mlpMid[20];
    BYTE        mlpType;
    BYTE        mlpServiceType;
    BYTE        mlpVendor;
    WORD        mlpDataCount;
    WORD        mlpLength;
    TIMESTAMP   mlpTime;
    char        mlpData[0];
}   __attribute__ ((packed)) METERLPENTRY;

/** Metering Data Handler.
  *
  * @author rewriter@nuritelecom.com
  *
  * @param pszAddress   Data를 전송한 Source Ip Adderss
  * @param pData        IF4_DATA_FRAME type의 data stream
  * @param nLength      data length
  */
void if4OnData(char *pszAddress, UINT nSourceId, BYTE *pData, int nLength) 
{
	IF4_DATA_FRAME	*pDataFrame;
	METERLPENTRY	*pEntry;
	char			*p;
	int				i;
    char            szId[64];

	pDataFrame = (IF4_DATA_FRAME *)pData;
	printf("IF4SERVER-METERING-DATA: MCUID=%d, Count=%0d\n",
			pDataFrame->sdh.mcuid,
			LittleToHostShort(pDataFrame->svch.cnt));

	p = pDataFrame->args;
	for(i=0; i<LittleToHostShort(pDataFrame->svch.cnt); i++)
	{
		pEntry = (METERLPENTRY *)p;
        EUI64ToStr(&pEntry->mlpId, szId);
		printf("%4d: [%s] [%04d/%02d/%02d %02d:%02d:%02d] Length=%0d\n",
				i+1, szId,
				LittleToHostShort(pEntry->mlpTime.year), pEntry->mlpTime.mon, pEntry->mlpTime.day,
				pEntry->mlpTime.hour, pEntry->mlpTime.min, pEntry->mlpTime.sec,
				LittleToHostShort(pEntry->mlpLength)-sizeof(TIMESTAMP));
		p += (LittleToHostShort(pEntry->mlpLength) + sizeof(METERLPENTRY) - sizeof(TIMESTAMP));
	}
}

void if4OnEvent(char *pszAddress, UINT nSourceId, BYTE *pEvent, int nLength)
{
}

void if4OnAlarm(char *pszAddress, UINT nSourceId, BYTE *pAlarm, int nLength)
{
}
