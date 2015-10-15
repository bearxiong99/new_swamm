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
extern VAROBJECT	m_omrEntry_node[];

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

typedef struct _tagCODIENTRY
{
	WORD        codiMask;
	BYTE        codiIndex;
	EUI64       codiID;
	BYTE        codiType;
	WORD        codiShortID;
	BYTE        codiFwVer;
	BYTE        codiHwVer;
	BYTE        codiZAIfVer;
	BYTE        codiZZIfVer;
	BYTE        codiFwBuild;
	BYTE        codiResetKind;
	BYTE        codiAutoSetting;
	BYTE        codiChannel;
	WORD        codiPanID;
	char        codiExtPanID[8];
	signed char codiRfPower;
	BYTE        codiTxPowerMode;
	BYTE        codiPermit;
	BYTE		codiEnableEncrypt;
	char        codiLineKey[16];
	char        codiNetworkKey[16];
    BOOL        codiRouteDiscovery;
    BOOL        codiMulticastHops;
} __attribute__ ((packed)) CODIENTRY;

typedef struct _tagDEVICEPARAM
{
	int			nFD;
	//
	char		szDevice[65];							// Device name
	int			nSpeed;									// Baudrate
	int			nDataBit;								// Data bit
	int			nStopBit;								// Stop bit
	int			nParity;								// Parity bit
} __attribute__ ((packed)) DEVICEPARAM;

typedef struct _tagOMRENTRY
{
	char		pMeterSerial[32];
    TIMESTAMP   nMetringTime;
    UINT        nCurrActiveEnergy;
    UINT        nCurrReactiveEnergy;
    UINT        nPrevActiveEnergy;
    UINT        nPrevReactiveEnergy;
    UINT        nPrevCumMaxTotal;
    UINT        nPowerFactor;
} __attribute__ ((packed)) OMRENTRY;

#endif	// __VARIABLE_H__
