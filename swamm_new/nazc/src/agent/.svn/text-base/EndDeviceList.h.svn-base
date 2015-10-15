#ifndef __END_DEVICE_LIST_H__
#define __END_DEVICE_LIST_H__

#include "Locker.h"
#include "TimeoutThread.h"
#include "Hash.h"
#include "Distributor.h"
#include "MeterParser.h"
#include "codiDefine.h"

#define OTA_STATE_INIT                  0
#define ENDI_VALUE_PATH                 "/app/value"

/** RF-3
  *
  * MBUS에 새로운 Meter Type이 추가되면서 LpPeriod가 없는 상태에서
  * (결국 몇개의 Frame이 올 지 모르는 상태에서) 검침데이터 완결 여부를
  * 판단해야 하는 경우가 생겼다. 이를 위해 신규 MBUS Frame의 경우 MSB를
  * Last Flag로 사용한다. 이 때 Old MBUS와의 호환성을 위해 LPPERIOD를 사용
  * 하는 방식과 미래 확장을 위해 Size를 사용하는 방식을 같이 정의하고
  * 검사 방법은 Mask로 결정한다.
  */
#define LAST_DATA_MASK_NONE             0x00
#define LAST_DATA_MASK_LPPERIOD         0x01
#define LAST_DATA_MASK_SIZE             0x02
#define LAST_DATA_MASK_MSB              0x10
#define LAST_DATA_MASK_LSB              0x20


/** ENDIENTRY에 잘못된 값이 들어 갔거나 잘못된 구조체를 사용할 경우 많은 문제가 될 수 있다.
  * 따라서 Magic Number를 넣어서 잘못된 Number일 경우 구조체를 지우게 한다. 이를 위해 magic
  * Attribute를 정의하고 ENDIENTRY_MAGIC_NUMBER를 define으로 정의한다. 이 define 값은 구조체가
  * 변경되어(크기 또는 attribute 위치) 이전 data를 Clear 해야 할 경우 증가시킨다. 만약 255보다
  * 커지게 되면 다시 0x01로 돌아가게 된다.
  *
#define ENDIENTRY_MAGIC_NUMBER          0x02    // Pulse Meter의 검침 Format을 변경하기 위한 저장용 Attribute 추가
#define ENDIENTRY_MAGIC_NUMBER          0x03    // #2021 : Pulse Metering Data Info Frame Length가 변경되었기 때문에 초기화
#define ENDIENTRY_MAGIC_NUMBER          0x04    // #2072 : Metering Fail Count에 의해 올라오는 검침데이터 처리를 위한 Attribute 추가
#define ENDIENTRY_MAGIC_NUMBER          0x05    // #1775 : M-BUS를 위한 Attribute 추가
#define ENDIENTRY_MAGIC_NUMBER          0x06    // #2499 : ACD에서 PQ 계산을 위한 Scale 값 항목 추가
#define ENDIENTRY_MAGIC_NUMBER          0x07    // #2571 : Sensor의 마지막 값을 저장하는 기능 추가
#define ENDIENTRY_MAGIC_NUMBER          0x08    // #RF-2 : DLMS Vendor 정보를 위해 nVendor 추가
#define ENDIENTRY_MAGIC_NUMBER          0x09    // #RF-2 : UINT형 Parser Type 추가
#define ENDIENTRY_MAGIC_NUMBER          0x0A    // Pulse형 Meter의 검침데이터에 Service Type이 각각 다르기 때문에 EndDevice별로 가지고 있게 한다.
#define ENDIENTRY_MAGIC_NUMBER          0x0B    // #65   : Energy Level 관리를 위한 property 추가
#define ENDIENTRY_MAGIC_NUMBER          0x0C    // #84   : Push Metering을 위해 예상되는 Metering Data Size 및 현재 Size에 해단 Attribute 추가
#define ENDIENTRY_MAGIC_NUMBER          0x0D    // #320  : Node Kind 항목 추가
  */
#define ENDIENTRY_MAGIC_NUMBER          0x0E    // #55   : Kamstrup Relay Control을 위한 nRelayStatus 추가

typedef struct    _tagENDIENTRY
{
        BYTE        magic;                      // ENDIENTRY Magic Number
        BYTE        nGroup;                     // Sensor Group Number by Do
        EUI64       id;                         // Sensor ID
        BYTE        nPort;                      // Port Number
        int         nAttribute;                 // Attribute

        // End Device information
        char        szNodeKind[33];             // End Device Node Kind String
        char        szParser[33];               // End Device Parser String
        UINT        nParser;                    // End Device Parser Type
        WORD        hw;                         // End Device 하드웨어 버젼
        WORD        fw;                         // End Device 펌웨어 버젼
        WORD        build;                      // 펌웨어 Build
        BOOL        bTestFlag;                  // Test Mode
        signed char nAlarmFlag;                 // Alarm Flag
        WORD        nAlarmMask;                 // Alarm Mask

        // Sub Device information              
        char        szSubModel[21];               // Sub Device Node Type
        WORD        subfw;                         // Sub Device 펌웨어 버젼
        WORD        subbuild;                      // 펌웨어 Build

        // Meter information
        char        szSerial[21];               // 미터 시리얼 번호
        char        szVendor[33];               // 미터 벤더
        BYTE        nVendor;                    // Vendor Code
        BYTE        nServiceType;               // Meter Service Type
        char        szModel[21];                // 미터 모델 명칭 (I210+c, ADNXXXX, ...)
        WORD        hwVersion;                  // 미터 하드웨어 버젼
        WORD        fwVersion;                  // 미터 펌웨어 버젼 
        UINT        meterError;                 // 미터 에러 코드
        BYTE        lastDataMask;               // 마지막 Frame을 알 수 있는 방법 (push 방식)

        // Route record
        WORD        parentid;                   // Parent node short-id
        WORD        shortid;                    // Short ID
        BYTE        hops;                       // Hop count
        WORD        path[CODI_MAX_ROUTING_PATH];// Routing path

        // End Device status
        BYTE        nState;                     // 현재 상태
        BOOL        bBusy;                      // 센서 사용중 플래그
        BOOL        bModified;                  // 정보 갱신 여부
        BOOL        bValueModified;             // Value 갱신 여부
        BOOL        bRouteModified;             // Routing record 갱신 여부

        // Energy Management
        BYTE        nEnergyLevel;               // Energy Level (0:undefined, 1~15:level)
        UINT        nRelayStatus;               // Relay Status (Kamstrup Cutoff state 등으로 사용 가능)

        // Pulse Meter LP Data Save Status
        BYTE        lpChoice;                   // lpChoice 
        BYTE        lpPeriod;                   // lpPeriod 
        BYTE        lpIndex;                    // current data index (Push에서는 현재 받아야 하는 LP Index로 사용)
        TIMEDATE    lpTimeDate;                 // current LP Time Date
        UINT        lpBasePulse;                // current LP base pulse
        UINT        nLastMeteringDate;          // Last metering day  year : 2byte, month : 1byte, day : 1byte {YYmd}
        UINT        nLastTempMeteringDate;      // Last Metering Date에 설정하기 전에 사용되는 임시값

        // Push Metering Attributes
        UINT        nTotalSize;                 // 전체 Metering Data Size
        UINT        nCurrentSize;               // 현재 까지 수집된 Metering Data Size

        cetime_t    tmChange;                   // 마지막 상태 변경 시간
        cetime_t    tmValueChange;              // 마지막 값 변경 시간
        cetime_t    tmMetering;                 // 마지막 검침 성공 시간
        cetime_t    tmMeteringFail;             // 마지막 검침 실패 시간
        cetime_t    tmEventLog;                 // 마지막 이벤트 로그 수집 시간
        cetime_t    tmRouteRecord;              // 마지막 라우팅 정보 변경 시간
        cetime_t    tmInstall;                  // 등록 시간
        cetime_t    tmUpload;                   // 마지막 Upload 성공 시간
        cetime_t    tmRouteDevice;              // 마지막으로 Route Device Information 변경된 시간
        cetime_t    tmSolarDevice;              // 마지막으로 Solar Device Information 변경된 시간
        cetime_t    tmLevelChange;              // Energy Level 변경된 마지막 시간
    
        // Firmware Update time
        UINT        lastSuccLength;             // 최종적으로 성공한 쓰기 길이 (이어쓰기 기능을 위해)
        time_t      tmLastFwUpdateTime;         // Firmware State 변경 시간
        BYTE        installState;               // Firmware Install State 
                                                // (0:Init, 1:Inventory, 2:Send, 4:Verify, 8:Install, 16: Scan)

        // Network Quality
        BYTE        LQI;                        // LQI
        signed char RSSI;                       // RSSI

        // Route Device Information
        BOOL        bPermitMode;                // Automatic Permit control(Enable/Disable)
        BOOL        bPermitState;               // Zigbee Permit
        BYTE        nMaxChildTbl;               // MAX Child Table Count
        BYTE        nChildTbl;                  // Current Child Table Count

        // Solar Battery Information
        WORD        nSolarAdVolt;               // Solar AD Volt
        WORD        nSolarChgBattVolt;          // Solar Charge Battery Volt
        WORD        nSolarBoardVolt;            // Solar Board Volt
        signed char nSolarChgState;             // Solar Battery Charging State (-1: undefined, 0: low, 1:high)

        // Constants
        double      fVoltageScale;              // ACD PQ Voltage Scale (Issue #2499)
        double      fCurrentScale;              // ACD PQ Current Scale (Issue #2499)

        MIBValue    *pMibValue;

        int         nPosition;
}    ENDIENTRY;

typedef BOOL (*PFNENUMENDDEVICE)(ENDIENTRY *pEndDevice, void *pParam, void *pParam1, void *pParam2);

class CEndDeviceList
{
public:
    CEndDeviceList();
    virtual ~CEndDeviceList();

// Attribute
public:
    int     GetCount();
    BOOL    IsBusy(const EUI64 *id);
    BOOL    IsModified(const EUI64 *id);

    BOOL    SetBusy(const EUI64 *id, BOOL bBusy=TRUE);
    BYTE    GetState(const EUI64 *id);
    BOOL    SetState(const EUI64 *id, BYTE nState);
    BOOL    SetInstallState(const EUI64 *id, BYTE state);

    BOOL    GetLastMeteringTime(const EUI64 *id, time_t *t);
    BOOL    SetLastMeteringTime(const EUI64 *id, time_t t);

    BOOL    GetLastMeteringFailTime(const EUI64 *id, time_t *t);
    BOOL    SetLastMeteringFailTime(const EUI64 *id, time_t t);

    BOOL    GetLastEventLogTime(const EUI64 *id, time_t *t);
    BOOL    SetLastEventLogTime(const EUI64 *id, time_t t);

    int     GetSensorAttribute(const EUI64 *id);

    BOOL    SetTempMeteringDate(const EUI64 *id, WORD year, BYTE month, BYTE day);
    BOOL    CommitMeteringDate(const EUI64 *id);
    BOOL    GetMeteringDate(const EUI64 *id, UINT *date, WORD *year, BYTE *month, BYTE *day);

    /** Metering Attribute Set */
    //BOOL    ResetMeteringAttribute();
    BOOL    SetMeteringAttribute(const EUI64 *id, int nPortNum, BYTE lastMask, BYTE period, BYTE choice, signed char alarmFlag);
    BOOL    SetMeteringDataSize(const EUI64 *id, UINT nTotalSize, BYTE nCurrentFrameLength);

    BOOL    SetPulseSensorLpTimePulse(const EUI64 *id, TIMEDATE * timedate, UINT basepulse);
    BOOL    IsValidLpIndex(const EUI64 *id, BYTE nIndex, UINT nPayloadSize, BYTE *current, BOOL *isLast);
    BOOL    IsDuplicateLpData(const EUI64 *id);
    BOOL    IsDuplicateLpData(const EUI64 *id, WORD year, BYTE month, BYTE day);

    ENDIENTRY *SetEndDeviceByID(const EUI64 *id, int nPortNum, const char *pszNodeKind,
            CMeterParser *pParser, const char *pszParser, UINT nParserType, int nAttribute);
    BOOL    UpdateSensorInventory(const EUI64 *id, const char *pszParser, UINT nParserType, 
            const char *pszModel, WORD hw, WORD fw, WORD build);
    BOOL    UpdateSubInventory(const EUI64 *id, const char *pszModel, WORD fw, WORD build);
    BOOL    SetEndDeviceFlag(const EUI64 *id, WORD alarmMask, BOOL alarmFlag, BOOL testFlag);
    BOOL    SetEndDeviceNetworkType(const EUI64 *id, BYTE networkType);
    BOOL    UpdateMeterInventory(const EUI64 *id, int nPortNum, int nParser, const char *pszVendor, int nVendor, 
                    const char *pszModel, const char *pszSerial, WORD hw, WORD fw);
    BOOL    UpdateConstantScale(const EUI64 *id, double fVoltageScale, double fCurrentScale);

    BOOL    SetServiceType(const EUI64 *id, BYTE nServiceType);

    BOOL    DeleteEndDeviceByID(const EUI64 *id);
    BOOL    DeleteEndDeviceAll();
    BOOL    VerifyEndDevice(const EUI64 *id);

    ENDIENTRY *GetEndDeviceByID(const EUI64 *id);
    ENDIENTRY *GetEndDeviceByShortID(WORD shortid);
    BOOL    CopyEndDeviceByID(const EUI64 *id, ENDIENTRY *endi);

    int     EnumEndDevice(PFNENUMENDDEVICE pfnCallback, void *pParam=NULL, void *pParam1=NULL, void *pParam2=NULL,
                    int(*cmpar)(const void *, const void *)=NULL);
    EUI64   *GetListByID(int *nCount, int nIncludeAttr, int nExcludeAttr);

    int		GetModelCount(const char *pszModel);
    //int		GetSubModelCount(const char *pszModel);
    OTANODE *GetOtaListByModel(const char *pszModel, int *nReturnCount, WORD oldHwVer, WORD oldFwVer, WORD oldBuild);
    OTANODE *GetOtaListById(const char *pszModel, int *nReturnCount, EUI64 *ids, int nCount, WORD oldHwVer, WORD oldFwVer, WORD oldBuild);

    BOOL    UpdateRouteRecord(const EUI64 *id, WORD shortid, BYTE hops, WORD *path); 
    BOOL    QueryRouteRecord(const EUI64 *id, WORD *shortid, BYTE *hops, WORD *path);
    BOOL    QueryRouteRecord(ENDIENTRY *endi, WORD *shortid, BYTE *hops, WORD *path, BOOL bLock=FALSE);

    BYTE    GetEndDeviceHops(ENDIENTRY *endi, BYTE *hops);

    BOOL    GetQuality(const EUI64 *id, BYTE * LQI, signed char * RSSI);
    BOOL    SetQuality(const EUI64 *id, BYTE LQI, signed char RSSI);

    BOOL    GetMeterError(const EUI64 *id, UINT erroType, BOOL *status);
    BOOL    SetMeterError(const EUI64 *id, UINT erroType, BOOL status);

    BOOL    SetRouteDeviceInfo(const EUI64 *id, BOOL bPermitMode, BOOL bPermitState, BYTE nMaxChildNode, BYTE nChildNode); 
    BOOL    SetSolarDeviceInfo(const EUI64 *id, WORD nSolarVolt, WORD nSolarChgBattVolt, WORD nSolarBoardVolt, signed char nSolarChgState);

    BOOL    ClearValue(const EUI64 *id);
    BOOL    AddValue(const EUI64 *id, const char *attrName, BYTE nType, UINT nLength, BYTE * pValue);

    BOOL    GetEnergyLevel(const EUI64 *id, BYTE *pEnergyLevel, UINT *pRelayStatus);
    BOOL    SetEnergyLevel(const EUI64 *id, BYTE nEnergyLevel, UINT nRelayStatus);
    BOOL    SetEnergyLevel(const EUI64 *id, BYTE nEnergyLevel, UINT nRelayStatus, BYTE *pszCallback, int nCallbackLength);

    BOOL    GetMeterSecurity(const EUI64 *id, BYTE* szBuffer, int* nLength);

    ENDIENTRY *FindEndDeviceByShortID(WORD shortid);
    ENDIENTRY *FindEndDeviceByID(const EUI64 *id);

public:
    static  int compAttrModelId(const void *p1, const void *p2);

// Operations
public:
    BOOL    Initialize();
    void    Destroy();
    void    Reset();

protected:
    BOOL    LoadEndDevice(const char *pszPath);
    BOOL    SaveEndDevice(ENDIENTRY *pEndDevice);
    BOOL    SaveEndDeviceValue(ENDIENTRY *pEndDevice);
    BOOL    RemoveAll();
    void    SetModify(ENDIENTRY *pEndDevice, BOOL bModified=FALSE);
    BOOL    CheckInternalStructure(const char *pszPath);

private:
    CHash                        m_Hash;
    CHash                        m_ShortIdHash;
    CLocker                        m_Locker;
    CLinkedList<ENDIENTRY *>    m_List;
};

extern CEndDeviceList    *m_pEndDeviceList;
#endif    // __END_DEVICE_LIST_H__
