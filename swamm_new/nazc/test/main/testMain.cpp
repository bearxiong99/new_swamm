
/** Nuritelecom AiMiR Project.
 *
 * Unit Test main function
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>

#include <fstream>
#include <UnitTest++.h>
#include <TestReporter.h>
#include <TestReporterStdout.h>
#include <XmlTestReporter.h>
#include <JUnitXmlReporter.h>

#include "typedef.h"
#include "DebugUtil.h"

void usage(char * name)
{
    fprintf(stderr,"%s [-rh]\n", name);
    fprintf(stderr,"    -r reporttype \n");
    fprintf(stderr,"       + TestReporterStdout (default)\n");
    fprintf(stderr,"       + XmlTestReporter\n");
    fprintf(stderr,"       + JUnitXmlReporter\n");
    fprintf(stderr,"    -s suite[:case][,suite[:case]] \n");
    fprintf(stderr,"    -l : display all test lists \n");
    fprintf(stderr,"    -d : enable remote console debug message\n");
    fprintf(stderr,"    -h : display this message\n");
    exit(1);
}

typedef struct _suiteTestName { const char * suiteName; const char * testName; } SuiteTestName;

struct TestNamePredicate
{
    TestNamePredicate(char *suite, char *test)
    {
        suiteName = suite;
        testName = test;
    }

    bool operator()(const UnitTest::Test* const curTest) const
    {
        if(suiteName) {
            if(!strcmp(suiteName, curTest->m_details.suiteName)) {
                if(testName) {
                    return strcmp(testName, curTest->m_details.testName) ? false : true;
                }
                return true;
            }
            return false;
        }
        return true;
    }

    char *suiteName;
    char *testName;
};

int CompName(const void *p1, const void *p2)
{
    SuiteTestName const * n1 = (SuiteTestName const *)p1;
    SuiteTestName const * n2 = (SuiteTestName const *)p2;

    int res = strcmp(n1->suiteName, n2->suiteName);
    if(!res) res = strcmp(n1->testName, n2->testName);

    return res;
}

int  runTest(UnitTest::TestReporter& reporter, char ** suites, int nSuite)
{
    int res = 0, i;
    UnitTest::TestRunner runner(reporter);

    if(nSuite <= 0) {
        return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);
    }else {
        for(i=0;i<nSuite;i++) {
            char * suite = suites[i];
            char * test = strchr(suite, ':');
            if(test) {
                test[0] = 0x00;
                test ++;
            }
            TestNamePredicate tnp(suite, test);

            res |= runner.RunTestsIf(UnitTest::Test::GetTestList(), suites[i], tnp, 0);
        }
    }
    return res;
}


int main(int argc, char * argv[])
{
    int res = 0;
	char fname[512] = {0,};
	char suite[512] = {0,};
	char *sp, *idx;
    char *suites = NULL, *tsuite;
    char *suiteList[2048] = {0, };
    char *reporterType = NULL;
    int opt, nDebug = 0;
    int nSuite = 0;
    int nList = 0;

    while((opt=getopt(argc, argv, "r:s:ldh")) != -1) {
        switch(opt) {
            case 'r': reporterType = strdup(optarg); break;
            case 's': suites = strdup(optarg); break;
            case 'l': nList = 1; break;
            case 'd': nDebug = 1; break;
            case 'h':
            default :
                usage(argv[0]);
        }
    }

    if(nList) {
        UnitTest::Test* head = UnitTest::Test::GetTestList().GetHead();
        UnitTest::Test* test = NULL;
        int nTestCnt = 0,i;
        SuiteTestName * nameArr = NULL;

        for(test = head; test != NULL; test = test->next) nTestCnt ++;

        nameArr = (SuiteTestName *) malloc(sizeof(SuiteTestName) * nTestCnt);
        for(test = head, i=0; test != NULL; test = test->next, i++) {
            nameArr[i].suiteName = test->m_details.suiteName;
            nameArr[i].testName = test->m_details.testName;
        }

        qsort(nameArr, nTestCnt, sizeof(SuiteTestName), CompName);

        for(i=0; i<nTestCnt; i++)
        {
            fprintf(stdout, "%s:%s\n", nameArr[i].suiteName, nameArr[i].testName);
        }
        free(nameArr);
        return 0;
    }

    if(suites) {
#ifndef _WIN32
        while((tsuite=strtok_r(suites,",", &suites))) {
#else
        while((tsuite=strtok(suites,","))) {
#endif
            suiteList[nSuite] = tsuite;
            nSuite ++;
        }
    }


    if(nDebug) SET_DEBUG_MODE(0);

    if(reporterType != NULL) {
        if(strcmp("XmlTestReporter", reporterType) && strcmp("JUnitXmlReporter", reporterType)) reporterType = NULL;
        else {
	        memset(fname, 0, sizeof(fname));
	        memset(suite, 0, sizeof(suite));
	        strncpy(fname, argv[0], strlen(argv[0]) < sizeof(fname) - 5 ? strlen(argv[0]) : sizeof(fname) - 5);
	        idx = rindex(fname,'.');
	        sp = rindex(fname,'/');

	        if(idx != NULL && idx != fname && (sp == NULL || idx > sp)) *idx = 0x00;
	        if(sp != NULL && *(sp + 1) != 0x00) strcpy(suite, sp + 1);
	        else strcpy(suite,fname);
	        strcat(fname, ".xml");
        }

    }

    if(reporterType == NULL) {
        UnitTest::TestReporterStdout reporter;
        res = runTest(reporter, suiteList, nSuite);
    }else if(!strcmp("XmlTestReporter", reporterType)) {
        std::ofstream f(fname);
        UnitTest::XmlTestReporter reporter(f);
        res = runTest(reporter, suiteList, nSuite);
    }else {
        std::ofstream f(fname);
        UnitTest::JUnitXmlReporter reporter(f, const_cast<char *>("nuri.aimir.concentrator"), suite);

        reporter.preAction();
        res = runTest(reporter, suiteList, nSuite);
        reporter.postAction();
    }

    return res;
}
