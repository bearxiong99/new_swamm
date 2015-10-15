#ifndef __QC_H__
#define __QC_H__

#define     DOT             20

#define		INIT_ERROR		        char _ebuff[256];
#define		ADD_ERROR(format,...)	sprintf(_ebuff, format, __VA_ARGS__);_addErrorMessage(_ebuff)

#define     INIT_DOT                int _dotCnt=0
#define     PARAM_DOT               &_dotCnt
#define     ARGU_DOT                int *_PdotCnt
#define     CHILD_DOT               int _dotCnt=*_PdotCnt
#define     SUCC_DOT(msg)           if(enableVerbose) printf(verbosePrefix "%s\n", msg); else printDot(1);_dotCnt++
#define     RETURN_DOT              *_PdotCnt=_dotCnt
#define     CLOSE_DOT               if(!enableVerbose) printDot(DOT-_dotCnt)

#define     errorPrefix             "            --> "
#define     verbosePrefix           "           + "


typedef void (*TPROC)();
typedef int (*TCOND)();

typedef struct	_tagTPROCTABLE
{
		const char	*pszTitle;
		TPROC	pfnFunc;
		TCOND	pfnCond;
}	TPROCTABLE, *PTPROCTABLE;

typedef struct _tagMessage
{
    char    *msg;
    struct _tagMessage * next;
}	Message;

void _rtcTest(void);    
void _gpioTest(void);  
void _mobileTest(void);
void _coordiTest(void);
void _heaterTest(void);
void _powerTest(void);

int _mobileCondition(void);
int _heaterCondition(void);

int openSerial(const char * ,  int , int );
void printDot(int);
void getEtherInfo(char *, UINT *, UINT *, BYTE *);

#endif
