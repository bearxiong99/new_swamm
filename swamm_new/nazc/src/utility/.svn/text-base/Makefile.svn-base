###########################################################################
##
## Copyright (c) 2008 NURI-Telecom Co., Ltd. 
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

TOP = ../..

include $(TOP)/Shared.rules
include $(TOP)/Makefile.rules

TARGET      = mcuCheck.exe          \
              putFile.exe           \
              getFile.exe           \
              mergeAndInstall.exe   \
              dumpSolarLog.exe      \
              ondemand.exe          \
              dumpMeteringData.exe  \
              mkPkgInfo.exe         \
              sensorList.exe         \
              macUtil.exe         \
              executeShell.exe      \
              executeMetering.exe

INCLUDES    =$(SYSINCPATH) -I.
INCLUDES   +=-I../include
INCLUDES   +=-I$(TOP)/include
INCLUDES   +=-I$(TOP)/include/common
INCLUDES   +=-I$(TOP)/include/cli
INCLUDES   +=-I$(TOP)/include/varapi
INCLUDES   +=-I$(TOP)/include/if4api
INCLUDES   +=-I$(TOP)/include/codiapi
INCLUDES   +=-I$(TOP)/include/protocol/code
INCLUDES   +=-I$(TOP)/include/protocol/dlms
INCLUDES   +=-I$(TOP)/include/protocol/mbus
INCLUDES   +=-I$(TOP)/include/protocol/kmp
INCLUDES   +=-I$(TOP)/core/gpio/driver/include
INCLUDES   +=-I../cli

LIB_PATH   +=-L$(TOP)/lib
LIBS       +=-lcommon$(CROSS_NAME)
LIBS       +=-lvarapi$(CROSS_NAME)
LIBS       +=-lif4api$(CROSS_NAME)
LIBS       +=-lz$(CROSS_NAME)
LIBS       +=-lpdlms$(CROSS_NAME)
LIBS       +=-lpkmp$(CROSS_NAME)
LIBS       +=-lpmbus$(CROSS_NAME)
LIBS       +=-lcode$(CROSS_NAME)
LIBS       +=-lssl
LIBS       +=-lcrypto
LIBS       +=-lsqlite3
LIBS       +=$(CLIBS)

CFLAGS      += -D__USE_STDOUT__
LDFLAGS     += 


SOURCES =   CommandHandler.cpp  \
            Utils.cpp  \
            CliUtils.cpp  \
            Message.cpp  \
            Variable.cpp 
OBJECTS = $(SOURCES:.cpp=.o)
LISTS = $(SOURCES:.cpp=.lst)

all: $(TARGET)
package: $(TARGET)

mcuCheck.exe: $(OBJECTS) mcuCheck.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) mcuCheck.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) mcuCheck.o $(LIB_PATH) $(LIBS) -o $@ 
endif

putFile.exe: $(OBJECTS) putFile.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) putFile.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) putFile.o $(LIB_PATH) $(LIBS) -o $@ 
endif

getFile.exe: $(OBJECTS) getFile.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) getFile.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) getFile.o $(LIB_PATH) $(LIBS) -o $@ 
endif

mergeAndInstall.exe: $(OBJECTS) mergeAndInstall.o
ifeq ($(strip $(LIB_EXT)),so)
else
endif
	$(CPP) $(LDFLAGS) $(OBJECTS) mergeAndInstall.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
	$(CPP) $(LDFLAGS) $(OBJECTS) mergeAndInstall.o $(LIB_PATH) $(LIBS) -o $@ 

dumpSolarLog.exe: $(OBJECTS) dumpSolarLog.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) dumpSolarLog.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) dumpSolarLog.o $(LIB_PATH) $(LIBS) -o $@ 
endif

ondemand.exe: $(OBJECTS) Ondemand.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) Ondemand.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) Ondemand.o $(LIB_PATH) $(LIBS) -o $@ 
endif

dump: dumpMeteringData.exe

dumpMeteringData.exe: DumpMeteringData.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) DumpMeteringData.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) DumpMeteringData.o $(LIB_PATH) $(LIBS) -o $@ 
endif

mkPkgInfo.exe: MkPkgInfo.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) MkPkgINfo.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
#	$(CPP) $(LDFLAGS) MkPkgInfo.o -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ $(LIB_PATH) $(LIBS) -lstdc++ -lpthread -lcommon$(CROSS_NAME) -lcrypt
else
	$(CPP) $(LDFLAGS) $(OBJECTS) MkPkgINfo.o $(LIB_PATH) $(LIBS) -o $@ 
#	$(CPP) $(LDFLAGS) MkPkgInfo.o -o $@ $(LIB_PATH) $(LIBS) -lstdc++ -lpthread -lcommon$(CROSS_NAME) -lcrypt
endif

sensorList.exe: getSensorList.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) getSensorList.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) getSensorList.o $(LIB_PATH) $(LIBS) -o $@ 
endif

executeShell.exe: $(OBJECTS) executeShell.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) executeShell.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) executeShell.o $(LIB_PATH) $(LIBS) -o $@ 
endif

executeMetering.exe: $(OBJECTS) executeMetering.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) executeMetering.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) executeMetering.o $(LIB_PATH) $(LIBS) -o $@ 
endif

macUtil.exe: $(OBJECTS) MacUtil.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) MacUtil.o $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) MacUtil.o $(LIB_PATH) $(LIBS) -o $@ 
endif

%.o: %.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

assemble: $(LISTS)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c -g -Wa,-a,-ad $<  > $@

clean:
	@rm -f $(TARGET)
	@rm -f *.o *.lst

