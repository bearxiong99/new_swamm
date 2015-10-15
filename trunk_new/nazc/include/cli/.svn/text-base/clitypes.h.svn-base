#ifndef __CLI_TYPEDEF_H__
#define __CLI_TYPEDEF_H__

#define CLITYPE_SERIAL					0				// Serial Console
#define CLITYPE_TELNET					1				// Telnet Console

#define CLIMODE_COMMAND					0				// Command Mode
#define CLIMODE_FORM					1				// Form Mode
#define CLIMODE_USER					2				// User Mode 
#define CLIMODE_ATTP					3				// ATTP Mode 

#if defined(__TI_AM335X__)
//#define CLIDEFAULT_SERIAL_DEVICE		"/dev/ttyO0"	// Serial Device
#define CLIDEFAULT_SERIAL_DEVICE		"/dev/console"	// Serial Device
#else
#define CLIDEFAULT_SERIAL_DEVICE		"/dev/ttyS2"	// Serial Device
#endif

#define CLIDEFAULT_TELNET_PORT			23				// Telnet Port
#define CLIUSER_FILENAME				"/app/conf/passwd"
#define CLISETUP_FILENAME				"/app/conf/cli.ini"
#define CLILOG_FILENAME					"/app/conf/syslog"
#define CLIDEFALUT_PROMPT				"AIMIR>"		// Default Command Prompt
#define CLIDEFALUT_USER_PROMPT			"User: "		// Default Login Prompt
#define CLIDEFALUT_PASSWORD_PROMPT		"Password: "	// Default Login Prompt
#define DEBUG_PROMPT					"Select: "		// Debugging Prompt

#define CLIDEFAULT_TIMEOUT				300				// Default Timeout Seconds = 5 Min

#define CLIMAX_SESSION_COUNT			10				// Maximum Session Count
#define CLIMAX_SERIAL_SESSION			1				// Maximum Serial Session Count
#define CLIMAX_PROMPT_SIZE				64				// Maximum Prompt Size
#define CLIMAX_COMMAND_SIZE				1024			// Maximum Command Size
#define CLIMAX_PARAM_COUNT				32				// Maximum Paramter Count
#define CLIMAX_OPTIONS					255				// Maximum Options
#define CLIMAX_VKEY_SIZE				10				// Maximum VKey Buffer Size
#define CLIMAX_HISTORY					20				// Maximum History Buffer Size
#define CLIMAX_LOGSAVEDAY				7				// Maximum Log Save Day

#define CLIERR_ERROR					-1				// Command Result Error
#define CLIERR_OK						0				// Command Result Ok
#define CLIERR_INVALID_COMMAND			1				// Invalid Command
#define CLIERR_INVALID_PARAM			2				// Invalid Parameter
#define CLIERR_MEMORY_ERROR				3				// Memory Error

#define CLIPROTOCOL_SERIAL				0				// Serial Protocol
#define CLIPROTOCOL_TCP					1				// TCP Telnet Protocol
#define CLIPROTOCOL_USB					2				// USB Connection

#define CLIGROUP_ADMIN					0				// Super User
#define CLIGROUP_USER					1				// Normal User
#define CLIGROUP_GUEST					2				// Guest User

#define CLI_LOGGING                     TRUE            // Logging
#define CLI_SKIPLOG                     FALSE           // Not Logging

#define CLI_NORMAL_CMD                  FALSE           // Normal Command
#define CLI_HIDDEN_CMD                  TRUE            // Hidden Command

#define ATTP_METHOD_CMD					0				// Command
#define ATTP_METHOD_SET					1				// Set Value
#define ATTP_METHOD_GET					2				// Get Value	
#define ATTP_METHOD_GETCHILD			3				// Get Child Value
#define ATTP_METHOD_PUT					4				// Put File

typedef struct	_tagCLIPROTOCOL
{
		int			nType;								// Protocol Type
		char		*pszName;							// Protocol Name
		char		*pszDevice;							// Device Name
		int			nPort;								// Port Number
		int			nMaxSession;						// Maximum Session Count
		int			nTimeout;							// Session Keepalive Time	
		BOOL		bStarted;							// Startup Flag
		void		*pFiber;							// Target Fiber
		int			nPosition;							// List Position
}	CLIPROTOCOL, *PCLIPROTOCOL;

typedef struct	_tagCLISESSION
{
		int			nType;								// Session Type
		int			nMode;								// Session Mode
		BOOL		bEmergency;							// Emergency Mode
		BOOL		bNeedUser;							// Account accept
		BOOL		bLogined;							// Login Flag
		BOOL		bDebugMode;
		BOOL		bBreak;								
		int			nLoginRetry;						// Login Retry Count
		long		lLastKeyinTime;						// Last Key Input Time
		char		*pszAddress;						// IP Address or TTY
		char		szPrompt[CLIMAX_PROMPT_SIZE+1];		// Prompt String
		char		szCommand[CLIMAX_COMMAND_SIZE+1];	// Command Buffer
		int			nCmdLength;							// Command Length
		int			nState;								// State
		int			argc;								// ARG Count
		char		*pszArgString;						// ARG Splite Buffer
		char		*argv[CLIMAX_PARAM_COUNT];			// ARG Buffer 
		int			nLocalOptions[CLIMAX_OPTIONS];		// Local Option Array
		int			nRemoteOptions[CLIMAX_OPTIONS];		// Remote Option Array
		int			nVKeyCount;							// Virtual Key Count
		int			nVKey;								// Current VKEY Code
		char		szVKeyBuffer[CLIMAX_VKEY_SIZE+1];	// Virtual Key Buffera
		char		szUser[17];							// User
		char		szPassword[17];						// Password
		int			nGroup;								// Group
		void		*pSession;							// Telnet Work Session
		void		*pProtocol;							// Protocol this Pointer
		void		*pWorker;							// Current worker
		void		*pInvoke;							// Current invoke
		char		*pszHistory[CLIMAX_HISTORY+1];		// History Buffer
		int			nHistoryCount;						// History Count
		int			nCurHistory;						// Current History Index
		int			nPosition;							// List Position
}	CLISESSION, *PCLISESION;

typedef int (*PFNCLIFUNC)(CLISESSION *pSession, int argc, char **argv, void *pHandler);
typedef int (*PFNCLICHECKFUNC)(CLISESSION *pSession, char *pszValue);

typedef struct	_tagCLIPARAMCHECKER
{
		const char		*pszFormat;							// Format 
		const char		*pszLabel;							// Label Name
		const char		*pszDescr;							// Description
		PFNCLICHECKFUNC	pfnCheck;						// Check Function
}	CLIPARAMCHECKER, *PCLIPARAMCHECKER;

typedef struct	_tagCLIHANDLER
{
		const char		*pszCommand;						// Command Name
		PFNCLIFUNC	pfnCommand;							// Execute Procedure
		struct		_tagCLIHANDLER *pNext;				// Next Entry Pointer
		const char		*pszParam;							// Parameter
		const char		*pszDescr;							// Command Description
		int			nGroup;								// Max Group
		BOOL		bLogFlag;							// Log Flag
        BOOL        bHidden;                            // Hidden Command
}	CLIHANDLER, *PCLIHANDLER;

typedef int (*PFNCLIFILTER)(CLISESSION *pSession, int argc, char **argv, int nIndex, void *pFilter);

typedef struct	_tagCLIFILTER
{
		char		*pszFilter;							// Fileter Format Name
		char		*pszShortName;						// Filter Short Name
		char		*pszFullName;						// Filter Full Name
		char		*pszDescr;							// Filter Descr
		PFNCLIFILTER pfnFilter;							// Filter Command Handler
}	CLIFILTER, *PCLIFILTER;

typedef struct	_tagCLISETTING
{
		BOOL		bEnableExpert;
}	CLISETTING, *PCLISETTING;

typedef void (*PFNONPARSEPARAMETER)(int argc, char **argv);
typedef void (*PFNONSTARTUP)();
typedef void (*PFNONSHUTDOWN)();
typedef void (*PFNONNEWSESSION)(CLISESSION *pSession);
typedef void (*PFNONDELETESESSION)(CLISESSION *pSession);
typedef BOOL (*PFNDOLOGIN)(CLISESSION *pSession, char *pszID, char *pszPassword);
typedef void (*PFNONLOGIN)(CLISESSION *pSession);
typedef void (*PFNONLOGOUT)(CLISESSION *pSession);
typedef void (*PFNONCOMMAND)(CLISESSION *pSession, int argc, char **argv, CLIHANDLER *pCmdTable);

typedef struct	_tagCLICONSTRUCT
{
		char					*pszVersion;			// MCU Version
		void					*pObject;
		PFNONPARSEPARAMETER		pfnOnParseParameter;
		PFNONSTARTUP			pfnOnStartup;
		PFNONSHUTDOWN			pfnOnShutdown;
		PFNONNEWSESSION			pfnOnEnterSession;
		PFNONDELETESESSION		pfnOnLeaveSession;
		PFNDOLOGIN				pfnDoLogin;
		PFNONLOGIN				pfnOnLogin;
		PFNONLOGOUT				pfnOnLogout;
		PFNONCOMMAND			pfnOnCommand;
}	CLICONSTRUCT, *PCLICONSTRUCT;

#endif	// __CLI_TYPEDEF_H__

