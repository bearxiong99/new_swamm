###########################################################################
##
## Copyright (c) 2009 NURI-Telecom Co., Ltd. 
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

TOP = ../../..
include $(TOP)/Shared.rules
include $(TOP)/Makefile.rules

TARGET      = cmdSendMessage.exe

INCLUDES    =$(SYSINCPATH) -I.
INCLUDES   +=-I$(TOP)/include
INCLUDES   +=-I$(TOP)/include/common
INCLUDES   +=-I$(TOP)/include/varapi
INCLUDES   +=-I$(TOP)/include/if4api

LIB_PATH   +=-L$(TOP)/lib

LIBS       +=-lif4api$(CROSS_NAME)
LIBS       +=-lvarapi$(CROSS_NAME)
LIBS       +=-lcommon$(CROSS_NAME)

ifeq ($(WINDOWS_FLAG),TRUE)
LIBS       +=-lpthread
LIBS       +=-lws2_32
endif

ifeq ($(DISABLE_SQLITE3),)
LIBS       +=-lsqlite3
endif
ifeq ($(DISABLE_SSL),)
LIBS       +=-lssl
LIBS       +=-lcrypto
endif
LIBS       +=-lz$(CROSS_NAME)
LIBS       +=$(CLIBS)

CFLAGS      +=
LDFLAGS     += 

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
LISTS = $(SOURCES:.cpp=.lst)

all: $(OBJECTS) $(TARGET)
package: $(OBJECTS) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CPP) $(LDFLAGS) $(INCLUDES) -o $@ $+ $(LIB_PATH) $(LIBS)

%.o: %.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c $<

assemble: $(LISTS)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c -g -Wa,-a,-ad $<  > $@

clean:
	@rm -f *.o *.exe *.lst
	@rm -rf docs

