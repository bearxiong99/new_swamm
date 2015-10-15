
#include <UnitTest++.h>

#include "common.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"


extern char gBaseName[];
const char *targetName = "SWTarget.file";
const char *sumName = "SWTarget.sum";
void if4OnDataHandler(char * szAddr, UINT nSourceId, BYTE nType, char *pszName, BYTE *pData, int nLength);

struct IF4DataFileFixture
{
    IF4DataFileFixture()
    {
        char buffer[1024 * 2];

        /** Callback 등록 */
        IF4API_SetUserCallback(if4OnDataHandler, NULL, NULL, NULL);
        /** targetName 설정 */
        sprintf(buffer,"cp %s %s", gBaseName, targetName);
        CHECK_EQUAL(0, system(buffer));
        /** CheckSum 생성 */
        sprintf(buffer,"md5sum %s > %s", targetName, sumName);
        CHECK_EQUAL(0, system(buffer));
        /** targetFile 삭제 */
        CHECK_EQUAL(0, remove(targetName));

    }
    ~IF4DataFileFixture() 
    {
        IF4API_SetUserCallback(NULL, NULL, NULL, NULL);

        CHECK_EQUAL(0, remove(targetName));
        CHECK_EQUAL(0, remove(sumName));
    }
};

void if4OnDataHandler(char * szAddr, UINT nSourceId, BYTE nType, char *pszName, BYTE *pData, int nLength)
{
    FILE *fp;

    if(!(fp = fopen(targetName, "wb"))) return;

    fwrite(pData, nLength, 1, fp); 
    fclose(fp);
}

/** Test Suite #99 IF4API Test.
 */
SUITE(_99_IF4API)
{

/** IF4API v1.1의 주요 기능인 sliding window를 이용한 파일 전송을 테스트 한다.
  *
  *     - 관련 Issue : #2049
 */
TEST_FIXTURE(IF4DataFileFixture, IF4API_SendDataFile)
{
    int nError;

    CIF4Invoke  * invoke = NULL;
    char buffer[1024 * 2];

    /** Send Data */
    {
        invoke = new CIF4Invoke("127.0.0.1", IF4_DEFAULT_PORT, 30);
        nError = IF4API_SendDataFile(invoke->GetHandle(), IF4_DATAFILE_GENERIC, gBaseName, TRUE);
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        sprintf(buffer,"cat %s | md5sum -c > /dev/null 2>&1", sumName);
        CHECK_EQUAL(0, system(buffer));
    }
}

} // End of Suite
