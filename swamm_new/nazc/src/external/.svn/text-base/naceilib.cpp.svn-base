
/** Nuritelecom AiMiR Project.
 *
 *  Nuri Aimir Concentrator External Interface Library.
 *
 */

#include "common.h"
#include "nacei.h"
#include "naceierr.h"

static char gServerAddr[64] = "127.0.0.1";


/** NACEI 초기화.
 * NACEI를 이용하기 위해서는 반드시 초기화 절차를 거쳐야 한다.
 * 자세한 사용법은 제공되는 @ref sample 를 참조한다.
 *
 * @author rewriter@nuritelecom.com
 * 
 * @return NACEI Error Code
 *
 * @retval NACEI_NOERROR 정상 종료
 * @retval NACE_INTERNAL_ERROR Internal Error
 */
int NACEI_Initialize_Interface(void)
{
    if(VARAPI_Initialize(NULL, m_Root_node, FALSE)) return NACEI_INTERNAL_ERROR;
    if(IF4API_Initialize(IF4_DEFAULT_PORT, NULL)) return NACEI_INTERNAL_ERROR;

    return NACEI_NOERROR;
}

/** NACEI 종료.
 * 종료 시 반드 시 Terminate 절차를 거친다 
 * 자세한 사용법은 제공되는 @ref sample 를 참조한다.
 *
 * @author rewriter@nuritelecom.com
 * 
 * @return NACEI Error Code
 *
 * @retval NACEI_NOERROR 정상 종료
 */
int NACEI_Terminate_Interface(void)
{
    IF4API_Destroy();
    VARAPI_Destroy();

    return NACEI_NOERROR;
}


/** NACEI Show Sensor List Command.
 * 
 * NAZC에서 제공하는 cmdShowSensorList를 이용한 센서 정보 조회. 
 * 자세한 사용법은 제공되는 @ref sample 를 참조한다.
 *
 * @author rewriter@nuritelecom.com
 *
 * @param[out] nCount Sensor 갯수
 * @param[out] pSensorInfo Sensor 정보. nCount가 0보다 클 때는 Memory Allocation
 *   이 되어 전달되기 때문에 사용 후에는 반드시 Free 해 주어야 한다.
 *
 * @return NACEI Error Code
 *
 * @retval NACEI_NOERROR 정상 종료
 * @retval NACE_INTERNAL_ERROR Internal Error
 * @retval NACE_MEMORY_ERROR Memory Allocation Error
 *
 * @warning NAZC에서 제공되는 Command를 호출할 때는 반드시 리턴값을 검사해야 한다.   
 *  리턴값이 NACEI_NOERROR가 아닐때 인자로 전달되는 값은 정의되지 않는다
 *
 */
int NACEI_Show_Sensor_List(int *nCount, NACEI_SENSOR_INFO ** pSensorInfo)
{
    CIF4Invoke  invoke(gServerAddr, IF4_DEFAULT_PORT, 3);
    IF4Invoke * pInvoke = NULL;
    SENSORINFOENTRYNEW  *_pSensor=NULL;
    BYTE        nAttr;
    int         nError;
    int         i;

    *nCount = 0; *pSensorInfo = NULL;

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nAttr  = IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE;
    nError = IF4API_Command(pInvoke, "102.26", nAttr);

    if(nError != IF4ERR_NOERROR) return NACEI_INTERNAL_ERROR;

    *nCount = pInvoke->nResultCount;

    if(*nCount > 0) {
        *pSensorInfo = (NACEI_SENSOR_INFO *) malloc(sizeof(NACEI_SENSOR_INFO)*(*nCount));
        if(*pSensorInfo == NULL) {
            return NACEI_MEMORY_ERROR;
        }
        memset(*pSensorInfo, 0, sizeof(NACEI_SENSOR_INFO)*(*nCount));
    }

    for(i=0; i<*nCount; i++)
    {
        _pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;
        IF4API_EUI64TOSTR((*pSensorInfo)[i].sensorID, &_pSensor->sensorID);
        memcpy((*pSensorInfo)[i].sensorSerialNumber, _pSensor->sensorSerialNumber, sizeof(_pSensor->sensorSerialNumber));
        memcpy(&(*pSensorInfo)[i].sensorLastConnect, &_pSensor->sensorLastConnect, sizeof(NACEI_TIME_STAMP));
        //memcpy(&(*pSensorInfo)[i].sensorInstallDate, &_pSensor->sensorInstallDate, sizeof(NACEI_TIME_STAMP));
        //memcpy(&(*pSensorInfo)[i].sensorAttr, &_pSensor->sensorAttr, sizeof(BYTE));
        memcpy(&(*pSensorInfo)[i].sensorState, &_pSensor->sensorState, sizeof(BYTE));
        memcpy((*pSensorInfo)[i].sensorModel, _pSensor->sensorModel, sizeof(_pSensor->sensorModel));
        //memcpy(&(*pSensorInfo)[i].sensorHwVersion, &_pSensor->sensorHwVersion, sizeof(WORD));
        //memcpy(&(*pSensorInfo)[i].sensorFwVersion, &_pSensor->sensorFwVersion, sizeof(WORD));
    }

    return NACEI_NOERROR;
}

