
#include "aimirTestReporter.h"
#include <cstdio>

#include "TestDetails.h"

// cstdio doesn't pull in namespace std on VC6, so we do it here.
#if defined(_MSC_VER) && (_MSC_VER == 1200)
namespace std {}
#endif

namespace UnitTest {

    AimirTestReporter::AimirTestReporter(void) {}
    AimirTestReporter::~AimirTestReporter(void) {}

    void AimirTestReporter::ReportFailure(TestDetails const& details, char const* failure)
    {
#if defined(__APPLE__) || defined(__GNUG__)
        char const* const errorFormat = "%s:%d: error: Failure in %s [%s]: %s\n";
#else
        char const* const errorFormat = "%s(%d): error: Failure in %s [%s]: %s\n";
#endif

        using namespace std;
        printf(errorFormat, details.filename, details.lineNumber, details.suiteName, details.testName, failure);
    }

    void AimirTestReporter::ReportTestStart(TestDetails const& details)
    {
    }

    void AimirTestReporter::ReportTestFinish(TestDetails const& details, float secondsElapsed)
    {
        printf("%s [%s] %.2f seconds\n",  details.suiteName, details.testName, secondsElapsed); 
    }

    void AimirTestReporter::ReportSummary(int const totalTestCount, int const failedTestCount,
            int const failureCount, float secondsElapsed)
    {
        using namespace std;
        if (failureCount > 0)
            printf("FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
        else
            printf("Success: %d tests passed.\n", totalTestCount);
        printf("Test time: %.2f seconds.\n", secondsElapsed);
    }

}

