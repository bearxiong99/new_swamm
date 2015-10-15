#ifndef __CMD_CTRL_H__
#define __CMD_CTRL_H__

int cmdResetCodi(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdResetMobile(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdResetSensor(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdAddSensor(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteSensor(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteMember(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteTransaction(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdMetering(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdOndemand(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdOndemandBypass(CLISESSION *pSession, int argc, char **argv, void *pHandler);
//int cmdOndemandRelay(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdOndemandCid(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdOndemandMccb(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdOndemandMbus(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdOndemandTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdMeterTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdQuery(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdOndemandMeterFailRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdCheckMeter(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdMulticastTime(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdMeterPingStart(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdMeterPingStop(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdAddApn(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteApn(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteMeter(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdAddCoordiRegister(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteCoordiRegister(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdSetSensorPort(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdGroupAsyncCall(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdGroupAdd(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdGroupDelete(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdGroupAddMember(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdGroupDeleteMember(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdGroupInfo(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdGroupShowCommand(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdGroupTableDelete(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdJointableShow(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdJointableAdd(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdJointableDelete(CLISESSION *pSession, int argc, char **argv, void *pHandler);

// Energy level 
int cmdSetEnergyLevel(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetIHDTable(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDelIHDTable(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteGroup(CLISESSION *pSession, int argc, char **argv, void *pHandler); // 실증단지

int cmdAddMeterSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteMeterSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdAddMeterProperty(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteMeterProperty(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdAddMeteringOption(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteMeteringOption(CLISESSION *pSession, int argc, char **argv, void *pHandler);

#ifdef __SUPPORT_KUKDONG_METER__
int cmdOndemandValve(CLISESSION *pSession, int argc, char **argv, void *pHandler);
#endif

#endif	// __CMD_CTRL_H__
