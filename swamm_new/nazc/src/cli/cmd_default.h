#ifndef __DEFAULT_COMMAND_H__
#define __DEFAULT_COMMAND_H__

int cmdInstall(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdUninstall(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdHelp(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdHistory(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdLogout(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdKillProcess(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdPing(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdShell(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdFirmwareDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdCoordinatorFirmwareDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdPowerControlFirmwareDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSensorFirmwareDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdScanSensor(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdPolicyDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdStore(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdDiagnosis(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdGarbageCleaning(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdReset(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdShutdown(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdFactoryDefault(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdAddUser(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDeleteUser(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetPassword(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdDisableAutoLogout(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableAutoLogout(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdDisableMonitoring(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableMonitoring(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdBackupConfiguration(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdBackupAll(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdRestore(CLISESSION *pSession, int argc, char **argv, void *pHandler);

#endif	// __DEFAULT_COMMAND_H__
