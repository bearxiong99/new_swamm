
TOP = ../..

include $(TOP)/Shared.rules
include $(TOP)/Makefile.rules

CXXFLAGS ?= -g -Wall -W -ansi -I../include # -pedantic
LDFLAGS ?= 
SED = sed
MV = mv
RM = rm

.SUFFIXES: .o .cpp

lib = $(TOP)/lib/libUnitTest++$(CROSS_NAME).a

src = src/AssertException.cpp \
	src/Test.cpp \
	src/Checks.cpp \
	src/TestRunner.cpp \
	src/TestResults.cpp \
	src/TestReporter.cpp \
	src/TestReporterStdout.cpp \
	src/ReportAssert.cpp \
	src/TestList.cpp \
	src/TimeConstraint.cpp \
	src/TestDetails.cpp \
	src/MemoryOutStream.cpp \
	src/DeferredTestReporter.cpp \
	src/DeferredTestResult.cpp \
	src/XmlTestReporter.cpp \
	src/JUnitXmlReporter.cpp \
	src/CurrentTest.cpp
	
ifeq ($(WINDOWS_FLAG),TRUE)
  src += src/Win32/TimeHelpers.cpp
  CXXFLAGS += -I../include/Win32
  CXXFLAGS += -I../include
else
  src += src/Posix/SignalTranslator.cpp \
	src/Posix/TimeHelpers.cpp
  CXXFLAGS += -I../include/Posix
  CXXFLAGS += -I../include
endif

objects = $(patsubst %.cpp, %.o, $(src))

all: $(lib)


$(lib): $(objects) 
	@echo Creating $(lib) library...
	@ar cr $(lib) $(objects)
    
clean:
	-@$(RM) -f $(objects) $(lib) 2> /dev/null

%.o : %.cpp
	@echo $<
	@$(CPP) $(CXXFLAGS) -c $< -o $(patsubst %.cpp, %.o, $<)


