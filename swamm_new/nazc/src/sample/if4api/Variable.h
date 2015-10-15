#ifndef __VARIABLE_H__
#define __VARIABLE_H__

extern VAROBJECT	m_Root_node[];
extern VAROBJECT	m_smi_node[];
extern VAROBJECT	m_smiEntry_node[];
extern VAROBJECT	m_unknownEntry_node[];
extern VAROBJECT	m_oidEntry_node[];
extern VAROBJECT	m_eui64Entry_node[];
extern VAROBJECT	m_ipEntry_node[];
extern VAROBJECT	m_timeEntry_node[];
extern VAROBJECT	m_dayEntry_node[];
extern VAROBJECT	m_verEntry_node[];
extern VAROBJECT	m_verService_node[];
extern VAROBJECT	m_smivarEntry_node[];
extern VAROBJECT	m_smivarOid_node[];
extern VAROBJECT	m_smivarLength_node[];
extern VAROBJECT	m_smivarData_node[];
extern VAROBJECT	m_pageEntry_node[];
extern VAROBJECT	m_gmtEntry_node[];

typedef struct _tagSENSORINFOENTRYNEW
{
	EUI64       sensorID;
	char        sensorSerialNumber[20];
	TIMESTAMP   sensorLastConnect;
	TIMESTAMP   sensorInstallDate;
	BYTE		sensorAttr;
	BYTE		sensorState;
	char		sensorModel[18];
	WORD		sensorHwVersion;
	WORD		sensorFwVersion;
	BYTE		sensorOTAState;
	TIMESTAMP   sensorLastOTATime;
	WORD		sensorFwBuild;
} __attribute__ ((packed)) SENSORINFOENTRYNEW;

#endif	// __VARIABLE_H__
