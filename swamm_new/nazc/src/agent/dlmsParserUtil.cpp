
#include "typedef.h"
#include "mcudef.h"

#include "dlmsFunctions.h"
#include "dlmsParserUtil.h"

#include "CommonUtil.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////


int makeEntrySelector(DLMSTAGVALUE *tagBuf, UINT nFrom, UINT nTo)
{
    BYTE bv;
    WORD wv;

    if(!tagBuf) return 0;
    tagBuf->nLength = 0;

    /** Issue #322 : DLMS Selector에서 1Byte 로 selector가 있는지 확인해 주는 값이 들어가야 한다
      * 이 부분에 대한 정확한 정보는 찾을 수 없지만 관련된 정보는 
      * http://www.dlms.com/faqanswers/forum/index.php?action=view&topicid=169
      * http://www.dlms.com/faqanswers/forum/index.php?action=view&topicid=156
      * 이곳에서 찾을 수 있다.
      */
    tagBuf->pPoint[tagBuf->nLength] = 1;        tagBuf->nLength++;  // Select 사용 유무 확인 (이게 왜 필요한지 모름)
    tagBuf->pPoint[tagBuf->nLength] = 2;        tagBuf->nLength++;  // Entry Descriptor 사용
    bv = 4; dlmsSetTagValue(tagBuf, DLMS_DATATYPE_STRUCTURE,(void *)&bv,    sizeof(BYTE));
            dlmsSetTagValue(tagBuf, DLMS_DATATYPE_UINT32,   (void *)&nFrom, sizeof(UINT));
            dlmsSetTagValue(tagBuf, DLMS_DATATYPE_UINT32,   (void *)&nTo,   sizeof(UINT));
    wv = 1; dlmsSetTagValue(tagBuf, DLMS_DATATYPE_UINT16,   (void *)&wv,    sizeof(WORD));
    wv = 6; dlmsSetTagValue(tagBuf, DLMS_DATATYPE_UINT16,   (void *)&wv,    sizeof(WORD));

    return tagBuf->nLength;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
int GetDlmsVendorToAimirVendor(BYTE code)
{
    int nVendor = METER_VENDOR_NOTSET;
    switch(code)
    {
        case 0x01: 
        case 0x0B: nVendor = METER_VENDOR_LSIS; break;
        case 0x02: 
        case 0x0C: nVendor = METER_VENDOR_PSTEC; break;
        case 0x03: 
        case 0x0D: nVendor = METER_VENDOR_KT; break;
        case 0x04: 
        case 0x0E: nVendor = METER_VENDOR_TAIHAN; break;
        case 0x05: 
        case 0x0F: nVendor = METER_VENDOR_SEOCHANG; break;
        case 0x06: 
        case 0x10: nVendor = METER_VENDOR_WIZIT; break;
        case 0x07: 
        case 0x11: nVendor = METER_VENDOR_CHUNIL; break;
        case 0x08: 
        case 0x12: nVendor = METER_VENDOR_NAMJUN; break;
        case 0x09: 
        case 0x13: nVendor = METER_VENDOR_ILJINE; break;
        case 0x15: nVendor = METER_VENDOR_AMRTECH; break;
        case 0x17: nVendor = METER_VENDOR_TGE; break;
        case 0x18: nVendor = METER_VENDOR_DMPOWER; break;
        case 0x19: nVendor = METER_VENDOR_AMSTECH; break;
        case 0x1A: nVendor = METER_VENDOR_OMNI; break;
        case 0x1B: nVendor = METER_VENDOR_MICRONIC; break;
        case 0x1C: nVendor = METER_VENDOR_HYUPSIN; break;
        case 0x1D: nVendor = METER_VENDOR_MSM; break;
        case 0x23: nVendor = METER_VENDOR_POWERPLUS; break;
        case 0x24: nVendor = METER_VENDOR_YPP; break;
        case 0x25: nVendor = METER_VENDOR_PYUNGIL; break;
        case 0x2F: nVendor = METER_VENDOR_NURI; break;
        case 0x33: 
        case 0x3D: nVendor = METER_VENDOR_AEG; break;
        case 0x46: 
        case 0x4B: nVendor = METER_VENDOR_GE; break;
        case 0x4D: nVendor = METER_VENDOR_ANSI; break;
        case 0x50: 
        case 0x55: nVendor = METER_VENDOR_LANDIS_GYR; break;
        case 0x5A: 
        case 0x5F: nVendor = METER_VENDOR_ETC; break;
    }

    return nVendor;
}

