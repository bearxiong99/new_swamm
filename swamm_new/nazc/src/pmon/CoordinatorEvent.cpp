#include "common.h"
#include "if4api.h"
#include "codiapi.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Data Definitions
//////////////////////////////////////////////////////////////////////
extern BOOL m_bBoot;
extern BYTE m_Fw;
extern BYTE m_Hw;
extern BYTE m_Build;
extern UINT m_nLocalAgentPort;

#define INFORMATION_TYPE_SHUTDOWN       0x81

//////////////////////////////////////////////////////////////////////
// Coordinator callback functions 
//////////////////////////////////////////////////////////////////////

void codiOnRegister(HANDLE codi, CODIDEVICE *device)
{
	// Coordinator가 등록될때 불린다.
}

void codiOnUnregister(HANDLE codi, CODIDEVICE *device)
{
	// Coordinator가 등록 해제될때 불린다.
}

void codiOnStartup(HANDLE codi)
{
    XDEBUG("Coordinator Startup\r\n");
}

void codiOnShutdown(HANDLE codi)
{
	// Coordinator가 Shutdown될때 불린다.
}

void codiOnBoot(HANDLE codi, EUI64 *id, WORD shortid, BYTE fw, BYTE hw, BYTE zaif, BYTE zzif, BYTE build, BYTE rk)
{
    m_bBoot = TRUE;
    m_Hw = hw;
    m_Fw = fw;
    m_Build = build;
}

void codiOnStackStatus(HANDLE codi, BYTE status)
{
}

void codiOnDeviceJoin(HANDLE codi, EUI64 *id, WORD shortid, WORD parentid, BYTE status)
{
}

void codiOnScanComplete(HANDLE codi, BYTE channel, BYTE status)
{
	// Coordinator는 Scan을 사용하지 않으므로 사용 하지 않는다.
}

void codiOnError(HANDLE codi, BYTE code)
{
	// Coordinator에서 명령을 처리할때 오류가 발생하면 불린다.
}

void codiOnCommand(HANDLE codi, BYTE ctrl, BYTE type, BYTE *pBuffer, int nLength)
{
	// 요청한 명령에 대한 응답이 온다.
}

void codiOnInformation(HANDLE codi, BYTE ctrl, BYTE type, BYTE *payload, int nLength)
{
    // Information Frame에 대한 User Handler
	XDEBUG("Information Frame (CTRL=0x%02X, TYPE=0x%02X, LEN=%d)\r\n", ctrl, type, nLength);
    XDUMP((char *)payload, nLength, TRUE);

    switch(type)
    {
        case INFORMATION_TYPE_SHUTDOWN:
            {
                CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);
                BYTE nAttr  = IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE;
                int nError = IF4API_Command(invoke.GetHandle(), "100.3", nAttr);
                XDEBUG("\033[1;40;31mPower Down Signal cached : Shutdown Concentrator (%s)\033[0m\r\n",
                        codiErrorMessage(nError));
            }
            break;
    }
}

void codiOnSend(HANDLE codi, BYTE ctrl, BYTE type, BYTE *payload, int nLength)
{
	// 자신이 전송한 메세지에 대한 콜백이 온다.
}

void codiOnCommandError(HANDLE codi, BYTE ctrl, BYTE cmd, BYTE code)
{
	// 요청한 명령에 대한 에러 코드가 온다.
}

void codiOnNoResponse(HANDLE codi, BOOL bResetNow)
{
	// Coordinator가 무응답일때 온다.
}

void codiOnRouteRecord(HANDLE codi, EUI64 *id, WORD shortid, BYTE hops, WORD *path)
{
	// 센서 목록에 Route record를 업데이트 한다.
}

BOOL codiOnQueryRouteRecord(HANDLE codi, EUI64 *id, WORD *shortid, BYTE *hops, WORD *path)
{
    return TRUE;
}

BOOL codiOnQuerySuperParent(EUI64 *id, EUI64 *parent, WORD *shortid, BYTE *hops, WORD *path)
{
    return TRUE;
}

void endiOnConnect(HANDLE codi, HANDLE endi)
{
	// End Device와 연결될때 온다.
}

void endiOnDisconnect(HANDLE codi, HANDLE endi)
{
	// End Device 접속이 끈길때 온다.
}

void codiOnEnergyScan(HANDLE codi, BYTE channel, BYTE rssi)
{
	// Coordinator에서 Scan하지 않는다.
}

void codiOnActiveScan(HANDLE codi, BYTE channel, WORD panid, BYTE *extpanid, BYTE expjoin, BYTE profile)
{
	// Coordinator에서 Active Scan은 하지 않는다.
}

void endiOnSend(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE type, BYTE seq, BYTE *payload, int nLength)
{
	// End Device에 데이터를 전송하면 온다.
	// endi == NULL 이면 MULTICAST, NULL이 아니면 UNICAST이다.
}

void endiOnError(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE type, BYTE code)
{
	// 통신에 대한 에러가 수신되는 부분
}

void endiOnBypass(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnNanData(HANDLE codi, EUI64 *id, BYTE reqid, BYTE flow, BYTE tailframe, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnPush(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnAmrData(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_AMR_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// AMR Data를 읽은 결과 Notify
}

void endiOnKeepAlive(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_AMR_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnNetworkLeave(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_LEAVE_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// 집중기는 Network를 Leave하지 않는다.
}

void endiOnRomRead(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_ROM_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// ROM Read response
}

void endiOnLongRomRead(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_LONG_ROM_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// ROM Read response
}

void endiOnMccbData(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_MCCB_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// Mccb 제어 결과 Notify
}

void endiOnCommand(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_COMMAND_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// EndiDevice Command response
}

void endiOnState(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_STATE_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI, WORD shortid)
{
}

void endiOnDiscovery(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_DISCOVERY_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnMbusData(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE mseq, BYTE mtype, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnUserDefine(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE type, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnMetering(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_METERING_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnEvent(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_EVENT_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

#if defined(__SUPPORT_DEVICE_NODE__)
void endiOnTimesync(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_TIMESYNC_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}
#endif

BOOL endiOnLink(HANDLE codi, EUI64 *id, BYTE type, BYTE LQI, signed char RSSI)
{
    return TRUE;
}

void endiOnRead(HANDLE codi, EUI64 *id, BYTE seq, WORD addr, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnData(HANDLE codi, EUI64 *id, BYTE seq, BYTE *data, int nLength, BYTE LQI, signed char RSSI)
{
}


