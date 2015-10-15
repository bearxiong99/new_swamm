#include "JUnitXmlReporter.h"
#include "Config.h"

#include <iostream>
#include <sstream>
#include <string>

#include <stdio.h>
#include <ctype.h>

using std::string;
using std::ostringstream;
using std::ostream;

namespace {

void ReplaceChar(string& str, char c, string const& replacement)
{
    for (size_t pos = str.find(c); pos != string::npos; pos = str.find(c, pos + 1))
        str.replace(pos, 1, replacement);
}

string XmlEscape(string const& value)
{
    string escaped = value;

    ReplaceChar(escaped, '&', "&amp;");
    ReplaceChar(escaped, '<', "&lt;");
    ReplaceChar(escaped, '>', "&gt;");
    ReplaceChar(escaped, '\'', "&apos;");
    ReplaceChar(escaped, '\"', "&quot;");
 
    return escaped;
}

string PrintEscape(string const& value)
{
	string r = "";
    const char * cv = value.data();
	int i, len = value.length();
	char buff[32];

	for(i=0;i<len;i++)
	{
		if(isprint(cv[i])) r.append(1, cv[i]);
		else {
			sprintf(buff,"0x%02X", cv[i]);
			r.append(buff);
		}
	}
 
    return r;
}

string BuildFailureMessage(string const& file, int line, string const& message)
{
    ostringstream failureMessage;
    failureMessage << file << "(" << line << ") : " << message;
    return failureMessage.str();
}

}

namespace UnitTest {

JUnitXmlReporter::JUnitXmlReporter(ostream& ostream, char *package, char * suite)
    : m_ostream(ostream), m_suite(suite)
{
    m_package = package;
    m_xmltag = false;
    m_testsuites = false;
    m_totalTestCount = 0;
    m_failedTestCount = 0;
    m_failureCount = 0;
    m_secondsElapsed = 0.0;
}

void JUnitXmlReporter::ReportSummary(int totalTestCount, int failedTestCount,
                                    int failureCount, float secondsElapsed)
{
    m_totalTestCount = totalTestCount;
    m_failedTestCount = failedTestCount;
    m_failureCount = failureCount;
    m_secondsElapsed = secondsElapsed;

    if(!m_testsuites) ReportOut(totalTestCount, failedTestCount, failureCount, secondsElapsed);
}

void JUnitXmlReporter::ReportOut(int totalTestCount, int failedTestCount,
                                    int failureCount, float secondsElapsed)
{
    AddXmlElement(m_ostream, NULL);

    BeginResults(m_ostream, m_package, m_suite, totalTestCount, failedTestCount, failureCount, secondsElapsed);

    DeferredTestResultList const& results = GetResults();
    for (DeferredTestResultList::const_iterator i = results.begin(); i != results.end(); ++i)
    {
        BeginTest(m_ostream, *i);

        if (i->failed)
            AddFailure(m_ostream, *i);

        EndTest(m_ostream, *i);
    }

    EndResults(m_ostream);
}

void JUnitXmlReporter::preAction()
{
    AddXmlElement(m_ostream, NULL);
    m_ostream << "<testsuites>\n";
    m_testsuites = true;
}

void JUnitXmlReporter::postAction()
{
    if(!m_testsuites) return;
    ReportOut(m_totalTestCount, m_failedTestCount, m_failureCount, m_secondsElapsed);
    m_ostream << "</testsuites>";
}

void JUnitXmlReporter::AddXmlElement(ostream& os, char const* encoding)
{
    if(m_xmltag) return;

    os << "<?xml version=\"1.0\"";

    if (encoding != NULL)
        os << " encoding=\"" << encoding << "\"";

    os << "?>\n";

    m_xmltag = true;
}

void JUnitXmlReporter::BeginResults(std::ostream& os, char * package, char * suite, int totalTestCount, int failedTestCount, 
                                   int failureCount, float secondsElapsed)
{
   os << " <testsuite"
       << " package=\"" << package << "\""
       << " name=\"" << suite << "\""
       << " tests=\"" << totalTestCount << "\"" 
       << " errors=\"" << failedTestCount << "\"" 
       << " failures=\"" << failureCount << "\"" 
       << " time=\"" << secondsElapsed << "\""
       << ">\n";
}

void JUnitXmlReporter::EndResults(std::ostream& os)
{
    os << " </testsuite>\n";
}

void JUnitXmlReporter::BeginTest(std::ostream& os, DeferredTestResult const& result)
{
    os << "  <testcase"
       << " classname=\"" << result.suiteName << "\"" 
       << " name=\"" << result.testName << "\""
       << " time=\"" << result.timeElapsed << "\""
       << ">";
}

void JUnitXmlReporter::EndTest(std::ostream& os, DeferredTestResult const& result)
{
    os << "</testcase><!-- " << result.testName << " -->\n";
}

void JUnitXmlReporter::AddFailure(std::ostream& os, DeferredTestResult const& result)
{
    for (DeferredTestResult::FailureVec::const_iterator it = result.failures.begin(); 
         it != result.failures.end(); 
         ++it)
    {
        string const escapedMessage = PrintEscape(XmlEscape(it->second));
        string const message = BuildFailureMessage(result.failureFile, it->first, escapedMessage);

		os << "   <error";
        os << " message=\"" << message << "\"" << "/>";
    }
}

}
