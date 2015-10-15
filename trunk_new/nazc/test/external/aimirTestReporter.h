#ifndef AIMIR_TEST_REPORTER
#define AIMIR_TEST_REPORTER

#include "TestReporter.h"

namespace UnitTest {

class AimirTestReporter : public TestReporter
{
    public:
        AimirTestReporter(void);
        ~AimirTestReporter(void);

        virtual void ReportTestStart(TestDetails const& test);
        virtual void ReportTestFinish(TestDetails const& test, float secondsElapsed);
        virtual void ReportFailure(TestDetails const& test, char const* failure);
        virtual void ReportSummary(int titalTestCount, int failedTestCount, int failureCount, float secondsElapsed);
};

}

#endif
