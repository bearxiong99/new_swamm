#ifndef __CODI_CALLBACK_H__
#define __CODI_CALLBACK_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Coordinator handler */

void codiOnRegister(HANDLE codi, CODIDEVICE *device);
void codiOnUnregister(HANDLE codi, CODIDEVICE *device);
void codiOnStartup(HANDLE codi);
void codiOnShutdown(HANDLE codi);

void codiOnBoot(HANDLE codi, EUI64 *id, WORD shortid, BYTE fw, BYTE hw, BYTE zaif, BYTE zzif, BYTE build, BYTE rk);
void codiOnStackStatus(HANDLE codi, BYTE status);
void codiOnDeviceJoin(HANDLE codi, EUI64 *id, WORD shortid, WORD parentid, BYTE status);
void codiOnEnergyScan(HANDLE codi, BYTE channel, BYTE rssi);
void codiOnActiveScan(HANDLE codi, BYTE channel, WORD panid, BYTE *extpanid, BYTE expjoin, BYTE profile);
void codiOnScanComplete(HANDLE codi, BYTE channel, BYTE status);
void codiOnError(HANDLE codi, BYTE code);
void codiOnNoResponse(HANDLE codi, BOOL bResetNow);
void codiOnRouteRecord(HANDLE codi, EUI64 *id, WORD shortid, BYTE hops, WORD *path);
BOOL codiOnQueryRouteRecord(HANDLE codi, EUI64 *id, WORD *shortid, BYTE *hops, WORD *path);

BOOL codiOnQuerySuperParent(EUI64 *id, EUI64 *parent, WORD *shortid, BYTE *hops, WORD *path);

void codiOnSend(HANDLE codi, BYTE ctrl, BYTE type, BYTE *payload, int nLength);
void codiOnCommand(HANDLE codi, BYTE ctrl, BYTE type, BYTE *pBuffer, int nLength);
void codiOnCommandError(HANDLE codi, BYTE ctrl, BYTE cmd, BYTE code);
void codiOnInformation(HANDLE codi, BYTE ctrl, BYTE type, BYTE *payload, int nLength);

/* End Device handler */

void endiOnConnect(HANDLE codi, HANDLE endi);
void endiOnDisconnect(HANDLE codi, HANDLE endi);
void endiOnSend(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE type, BYTE seq, BYTE *payload, int nLength);
void endiOnError(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE type, BYTE code);

void endiOnBypass(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnAmrData(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_AMR_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnNetworkLeave(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_LEAVE_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnRomRead(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_ROM_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnMccbData(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_MCCB_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnMetering(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_METERING_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnMbusData(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE mseq, BYTE mtype, BYTE *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnEvent(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_EVENT_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnKeepAlive(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_AMR_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnLongRomRead(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_LONG_ROM_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnCommand(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_COMMAND_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnState(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_STATE_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI, WORD shortid);
void endiOnDiscovery(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_DISCOVERY_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnUserDefine(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE type, BYTE *payload, int nLength, BYTE LQI, signed char RSSI);
#if defined(__SUPPORT_DEVICE_NODE__)
void endiOnTimesync(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_TIMESYNC_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI);
#endif
void endiOnNanData(HANDLE codi, EUI64 *id, BYTE reqid, BYTE flow, BYTE tailframe, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI);
void endiOnPush(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI);

/* End Device trap connection handler */
BOOL endiOnLink(HANDLE codi, EUI64 *id, BYTE type, BYTE LQI, signed char RSSI);
void endiOnRead(HANDLE codi, EUI64 *id, BYTE seq, WORD addr, int nLength, BYTE LQI, signed char RSSI);
void endiOnData(HANDLE codi, EUI64 *id, BYTE seq, BYTE *data, int nLength, BYTE LQI, signed char RSSI);

#ifdef __cplusplus
}
#endif

#endif	/* __CODI_CALLBACK_H__ */

