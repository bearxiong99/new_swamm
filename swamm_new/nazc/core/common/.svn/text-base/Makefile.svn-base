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

LIB_NAME    = libcommon$(CROSS_NAME).${LIB_EXT}
MLIB_NAME   = libmcommon$(CROSS_NAME).${LIB_EXT}
TARGET      = $(TOP)/lib/$(LIB_NAME)
MTARGET     = $(TOP)/lib/$(MLIB_NAME)
INCLUDES    = $(SYSINCPATH) -I. -I$(TOP)/include
INCLUDES   +=-I$(TOP)/include/common

LIB_PATH    =-L$(TOP)/lib

SHARED_LIB_PATH    =$(TOP)/bin/lib

TESTDIR     = $(TOP)/test
TESTSRCDIR  = $(TOP)/test/core/common
TESTLIB     = -lUnitTest++$(CROSS_NAME) 
TESTLIBS    = $(TESTLIB)
TESTLIBS   +=-lsqlite3
TESTLIBS   +=-lssl
TESTLIBS   +=-lcrypto
TESTLIBS   +=-lz$(CROSS_NAME)
TESTLIBS   +=$(CLIBS)
INCLUDES   += -I$(TOP)/test/include

TESTSRC     = $(wildcard $(TESTSRCDIR)/*.cpp)
TESTSRC    += $(wildcard $(TESTDIR)/main/*.cpp)
TESTOBJS    = $(TESTSRC:.cpp=.o)

TESTTARGET  = $(TOP)/bin/test/testCoreCommon.exe
TESTRESULT  = $(TESTTARGET:.exe=.xml)

SOURCES = $(wildcard [a-zA-Z]*.cpp)
MSOURCES =  CRC16.cpp           \
            TCPClient.cpp       \
            TCPSocket.cpp       \
            TCPMultiplexServer.cpp       \
            Hash.cpp            \
            MemoryDebug.cpp     \
            Queue.cpp           \
            BCC.cpp             \
            DataStream.cpp      \
            Options.cpp         \
            Chunk.cpp           \
            Parity.cpp          \
            CommonUtil.cpp      \
            DebugUtil.cpp       \
            Locker.cpp          \
            MD5.cpp             

ifeq ($(WINDOWS_FLAG),TRUE)
SOURCES     += $(wildcard _Win*.cpp)
MSOURCES     += $(wildcard _Win*.cpp)
else
SOURCES     += $(wildcard _Unix*.cpp)
MSOURCES     += $(wildcard _Unix*.cpp)
endif


OBJECTS = $(SOURCES:.cpp=.o)
MOBJECTS = $(MSOURCES:.cpp=.o)
LISTS = $(SOURCES:.cpp=.lst)

all: $(TARGET) $(MTARGET)
package: $(TARGET) $(MTARGET)

$(TARGET): $(SOURCES) $(OBJECTS)
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) -shared -Wl,-soname,$(LIB_NAME) $(OBJECTS) -o $(TARGET)
	cp -f $(TARGET) $(SHARED_LIB_PATH)/$(LIB_NAME)
else
	$(AR) rc $(TARGET) $(OBJECTS)
endif

$(MTARGET): $(MSOURCES) $(MOBJECTS)
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) -shared -Wl,-soname,$(MLIB_NAME) $(MOBJECTS) -o $(MTARGET)
	cp -f $(MTARGET) $(SHARED_LIB_PATH)/$(MLIB_NAME)
else
	$(AR) rc $(TARGET) $(MOBJECTS)
endif

%.o:	%.cpp
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(CFLAGS) $(INCLUDES) -fPIC -c $< -o $@
else
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@
endif

test: $(TESTLIB) $(TESTTARGET)

$(TESTTARGET): $(OBJECTS) $(TESTOBJS)
	$(CPP) $(OBJECTS) $(TESTOBJS) -o $@ $(LIB_PATH) $(TESTLIBS) 

$(TESTLIB): 
	$(MAKE) -C $(TESTDIR)/UnitTest++

testrun: test
	$(TESTTARGET)

assemble: $(LISTS)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c -g -Wa,-a,-ad $<  > $@

clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(MTARGET)" ]; then rm -f $(MTARGET); fi
	@if [ -f "$(SHARED_LIB_PATH)/$(LIB_NAME)" ]; then rm -f $(SHARED_LIB_PATH)/$(TARGET); fi
	@if [ -f "$(SHARED_LIB_PATH)/$(MLIB_NAME)" ]; then rm -f $(SHARED_LIB_PATH)/$(MTARGET); fi
	@if [ -f "$(TESTTARGET)" ]; then rm -f $(TESTTARGET); fi
	@if [ -f "$(TESTRESULT)" ]; then rm -f $(TESTRESULT); fi
	@rm -f *.o *.lst
	@rm -f $(TESTSRCDIR)/*.o

