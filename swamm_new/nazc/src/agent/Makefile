###########################################################################
##
## Copyright (c) 2004 NURI-Telecom Co., Ltd. 
## All rights reserved. 
##
## Redistribution and use in source and binary forms, with or without 
## modification, are permitted provided that the following conditions are met: 
##
## * Redistributions of source code must retain the above copyright notice, 
## this list of conditions and the following disclaimer. 
## * Redistributions in binary form must reproduce the above copyright notice, 
## this list of conditions and the following disclaimer in the documentation 
## and/or other materials provided with the distribution. 
## * Neither name of Intel Corporation nor the names of its contributors 
## may be used to endorse or promote products derived from this software 
## without specific prior written permission.
## 
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
## ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR 
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
## OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
## NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
## SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
###########################################################################

TOP         = ../..

include $(TOP)/Shared.rules
include $(TOP)/Makefile.rules

TARGET      =$(TOP)/bin/agent
INCLUDES    =$(SYSINCPATH) -I.
INCLUDES   +=-I$(TOP)/include
INCLUDES   +=-I$(TOP)/include/common
INCLUDES   +=-I$(TOP)/include/gpio
INCLUDES   +=-I$(TOP)/include/codiapi
INCLUDES   +=-I$(TOP)/include/if4api
INCLUDES   +=-I$(TOP)/include/varapi
INCLUDES   +=-I$(TOP)/include/gdbm
INCLUDES   +=-I$(TOP)/include/sqlite
INCLUDES   +=-I$(TOP)/core/gpio/driver/include
INCLUDES   +=-I$(TOP)/include/protocol/code

#INCLUDES   +=-Iparser
#INCLUDES   +=-Irep
INCLUDES   +=-I$(TOP)/include/protocol/ansi
INCLUDES   +=-I$(TOP)/include/protocol/iec62056_21
INCLUDES   +=-I$(TOP)/include/protocol/dlms
INCLUDES   +=-I$(TOP)/include/protocol/mbus
INCLUDES   +=-I$(TOP)/include/protocol/kmp
INCLUDES   +=-I$(TOP)/include/protocol/osaki

LIB_PATH    =-L$(TOP)/lib


LIBS       +=-lpansi$(CROSS_NAME)
LIBS       +=-lpiec62056_21$(CROSS_NAME)
LIBS       +=-lpdlms$(CROSS_NAME)
LIBS       +=-lpmbus$(CROSS_NAME)
LIBS       +=-lpkmp$(CROSS_NAME)
LIBS       +=-lposaki$(CROSS_NAME)
LIBS       +=-lcode$(CROSS_NAME)

LIBS       +=-lif4api$(CROSS_NAME)
LIBS       +=-lvarapi$(CROSS_NAME)
LIBS       +=-lcodiapi$(CROSS_NAME)
ifneq ($(CROSS_NAME),)
LIBS       +=-lgpio$(CROSS_NAME)
endif
LIBS       +=-lcommon$(CROSS_NAME)

LIBS       +=-lsqlite3
LIBS       +=-lssl
LIBS       +=-lcrypto
LIBS       +=-lz$(CROSS_NAME)

LIBS       +=$(CLIBS)

LDFLAGS    += 
CFLAGS     += 

TESTDIR     = $(TOP)/test
TESTSRCDIR  = $(TOP)/test/src/agent
TESTLIB     = -lUnitTest++$(CROSS_NAME) 
TESTLIBS    = $(TESTLIB)
TESTLIBS   += $(LIBS)
INCLUDES   += -I$(TOP)/test/include

SOURCES = $(wildcard *.cpp)
SOURCES += $(wildcard parser/*.cpp)
SOURCES += $(wildcard rep/*.cpp)

OBJECTS = $(SOURCES:.cpp=.o)
LISTS = $(SOURCES:.cpp=.lst)

TESTSRC     = $(wildcard $(TESTSRCDIR)/*.cpp)
TESTSRC    += $(wildcard $(TESTSRCDIR)/rep/*.cpp)
TESTSRC    += $(wildcard $(TESTSRCDIR)/parser/*.cpp)
TESTSRC    += $(wildcard $(TESTDIR)/main/*.cpp)
TESTSRC    += $(filter-out agent.cpp,$(SOURCES))
TESTOBJS    = $(TESTSRC:.cpp=.o)

TESTTARGET  = $(TOP)/bin/test/testAgent.exe
TESTRESULT  = $(TESTTARGET:.exe=.xml)


all: revision.h $(TARGET)
package: revision regrevision $(TARGET)
$(TARGET): $(SOURCES) $(OBJECTS)
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -o $@
endif

%.o:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: revision.h $(TESTLIB) $(TESTTARGET)

$(TESTTARGET): $(TESTSRC) $(TESTOBJS)
	$(CPP) $(TESTOBJS) -o $@ $(LIB_PATH) $(TESTLIBS) 

$(TESTLIB): 
	$(MAKE) -C $(TESTDIR)/UnitTest++

testrun: test
	$(TESTTARGET)

assemble: $(LISTS)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c -g -Wa,-a,-ad $<  > $@

revision.h: revision
revision:
	@$(TOP)/bin/revision.sh > revision.h
regrevision:
	@touch Variable.cpp
	
dev: all
	@sudo cp $(TARGET) /home/gloscom/nfs/new_sweden_151013/


clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@rm -f *.o *.lst
	@rm -f parser/*.o parser/*.lst
	@rm -f rep/*.o rep/*.lst

