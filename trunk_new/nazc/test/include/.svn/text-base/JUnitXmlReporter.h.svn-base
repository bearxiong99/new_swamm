#ifndef UNITTEST_JUNITXMLREPORTER_H
#define UNITTEST_JUNITXMLREPORTER_H

#include "DeferredTestReporter.h"

#include <iosfwd>

namespace UnitTest
{

class JUnitXmlReporter : public DeferredTestReporter
{
public:
    explicit JUnitXmlReporter(std::ostream& ostream, char *package, char *suit);

    void preAction();
    void postAction();

    virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);

private:
    JUnitXmlReporter(JUnitXmlReporter const&);
    JUnitXmlReporter& operator=(JUnitXmlReporter const&);

    void AddXmlElement(std::ostream& os, char const* encoding);
    void BeginResults(std::ostream& os, char *package, char *suite, int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);
    void EndResults(std::ostream& os);
    void BeginTest(std::ostream& os, DeferredTestResult const& result);
    void AddFailure(std::ostream& os, DeferredTestResult const& result);
    void EndTest(std::ostream& os, DeferredTestResult const& result);
    void ReportOut(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);

    std::ostream& m_ostream;
	char *m_suite;
	char *m_package;
    bool m_xmltag;
    bool m_testsuites;
    int  m_totalTestCount;
    int  m_failedTestCount;
    int  m_failureCount;
    float m_secondsElapsed;
};

}

#endif
