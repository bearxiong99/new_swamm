
/** @example sample.cpp
 *
 * Concentrator External Interface Sample Code
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "nacei.h"
#include "naceierr.h"


/** NACEI Sample Code 
 * 
 */
int main(int argc, char * argv[])
{
    int nError = NACEI_NOERROR;
    NACEI_SENSOR_INFO * pInfo = NULL;
    int nCount = 0;
    int i;

	/** NACEI 초기화.
     *  NACEI를 이용하기 위해서는 반드시 초기화 절차를 거쳐야 한다.
     */
    nError = NACEI_Initialize_Interface();
    if(nError != NACEI_NOERROR) {
        fprintf(stderr,"NACEI Initalize Fail:%d\n", nError);
        return nError;
    }

	/** NAZC에서 제공하는 cmdShowSensorList를 이용한 센서 정보 조회.
     *  NAZC에서 제공되는 Command를 호출할 때는 반드시 리턴값을 검사해야 한다.
     *  리턴값이 NACEI_NOERROR가 아닐때 인자로 전달되는 값은 정의되지 않는다
     */
    if((nError=NACEI_Show_Sensor_List(&nCount,&pInfo))!= NACEI_NOERROR) {
        fprintf(stderr,"NACEI Interface Call Error:%d\n", nError);
        return nError;
    }

    fprintf(stdout,"nCount=%d\n", nCount);
    fprintf(stdout, "---------------------------------------------------------\n");
    for(i=0;i<nCount;i++) {
        fprintf(stdout, "        Model : %s\n", pInfo[i].sensorModel);
        fprintf(stdout, "    Sensor ID : %s\n", pInfo[i].sensorID);
        fprintf(stdout, " SerialNumber : %s\n", pInfo[i].sensorSerialNumber);
        fprintf(stdout, "  LastConnect : %04d/%02d/%02d %02d:%02d:%02d\n", 
            pInfo[i].sensorLastConnect.year,
            pInfo[i].sensorLastConnect.mon,
            pInfo[i].sensorLastConnect.day,
            pInfo[i].sensorLastConnect.hour,
            pInfo[i].sensorLastConnect.min,
            pInfo[i].sensorLastConnect.sec );
        fprintf(stdout, "        State : %s\n", pInfo[i].sensorState ? "Abnormal" : "Normal");
        fprintf(stdout, "---------------------------------------------------------\n");
    }

	/** Memory Allocation 되는 값은 항상 검사해서 Free 해 준다 */
    if(pInfo) free(pInfo);

	/** 종료 시 반드 시 Terminate 절차를 거친다 */
    return NACEI_Terminate_Interface();
}
