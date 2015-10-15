#ifndef __TEST_COMMAND_H__
#define __TEST_COMMAND_H__

int cmdExit(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdTestGetAll(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdTestSetAll(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEventTest(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdMeteringAll(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdMeterUpload(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdGetMeterSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdPageRead(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdPageReadAll(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdPageWrite(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdGetConfig(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetConfig(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdUpgradeCancel(CLISESSION *pSession, int argc, char **argv, void *pHandler);

#endif	// __TEST_COMMAND_H__
