#include "common.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "kmpFrame.h"
#include "kmpDefine.h"

#include "KmpCrcCheck.h"
#include "KmpDataFrame.h"

int MakeKmpTail(BYTE *pszBuffer, BYTE len, KMP_TAIL *pTail)
{
	pTail->crc      = BigToHostShort(KmpGetCrc(pszBuffer, len));
	pTail->stop     = KMP_EOF;

    return sizeof(KMP_TAIL);
}

int MakeKmpHeader(KMP_HEADER *pHeader, BYTE addr, BYTE cid)
{
	memset(pHeader, 0, sizeof(KMP_HEADER));
	pHeader->start      = KMP_SOF_REQ;
    pHeader->addr       = addr;
    pHeader->cid        = cid;

    return sizeof(KMP_HEADER);
}

int MakeKmpFrame(BYTE *pszBuffer, BYTE cid, BYTE *pszData, int nLength)
{
    BYTE szTmpBuff[256];
    int len = 0;

    if(nLength < 0) return 0;

    memset(szTmpBuff, 0, sizeof(szTmpBuff));

    len += MakeKmpHeader((KMP_HEADER *)szTmpBuff, 0x3F, cid);
    if(pszData && nLength) 
    {
        memcpy(szTmpBuff+len, pszData, nLength);
        len += nLength;
    }
    len += MakeKmpTail(szTmpBuff+1, len - 1, (KMP_TAIL *)&szTmpBuff[len]);   // SOF 제외, Addr/CID 포함

    len = StuffFrame(szTmpBuff+1, len - 2, pszBuffer+1);    // SOF/EOF 제외, Addr/CID/CRC 포함
    pszBuffer[0] = KMP_SOF_REQ;
    pszBuffer[len+1] = KMP_EOF;

    return len + 2;     // SOF,EOF 길이 포함
}

/** SOF 다음부터 CRC 전까지 전달되어야 한다.
  */
int StuffFrame(BYTE *pszBuffer, int nLength, BYTE *stuffedBuffer)
{
    int i=0, resIdx=0;

    for(; i < nLength; i++, resIdx++)
    {
        switch(pszBuffer[i])
        {
            case KMP_SOF_REQ:
            case KMP_SOF_RES:
            case KMP_EOF:
            case KMP_ACK:
            case KMP_CTR:
                stuffedBuffer[resIdx] = KMP_CTR;
                stuffedBuffer[resIdx+1] = (pszBuffer[i] & 0xFF) ^ 0xFF;
                resIdx++;
                break;
            default:
                stuffedBuffer[resIdx] = pszBuffer[i];
                break;
        }
    }

    return resIdx;
}

int StripFrame(BYTE *pszBuffer, int nLength, BYTE *stripBuffer)
{
    int i=0, resIdx=0;

    for(; i < nLength; i++, resIdx++)
    {
        if(pszBuffer[i] == KMP_CTR)
        {
            stripBuffer[resIdx] = (pszBuffer[i+1] & 0xFF) ^ 0xFF;
            i++;
        }
        else
        {
            stripBuffer[resIdx] = pszBuffer[i];
        }
    }

    return resIdx;
}
