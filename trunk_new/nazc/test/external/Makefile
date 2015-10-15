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

TOP         =../..
include $(TOP)/Shared.rules
include $(TOP)/Makefile.rules

TARGET      =testExternal.exe
INCLUDES    =$(SYSINCPATH) -I.
INCLUDES   +=-I../include
INCLUDES   +=-I$(TOP)/include
INCLUDES   +=-I$(TOP)/include/common
INCLUDES   +=-I$(TOP)/include/varapi
INCLUDES   +=-I$(TOP)/include/if4api
INCLUDES   +=-I$(TOP)/include/codiapi
INCLUDES   +=-I$(TOP)/core/gpio/driver/include

LIB_PATH    =-L$(TOP)/lib

LIBS       +=-lUnitTest++$(CROSS_NAME)
LIBS       +=-lvarapi$(CROSS_NAME)
LIBS       +=-lif4api$(CROSS_NAME)

ifeq ($(DISABLE_SSL),)
LIBS       +=-lcommon$(CROSS_NAME)
LIBS       +=-lssl
LIBS       +=-lcrypto
else
LIBS       +=-lmcommon$(CROSS_NAME)
endif
ifeq ($(DISABLE_SQLITE3),)
LIBS       +=-lsqlite3
endif

LIBS       +=$(CLIBS)

ifeq ($(WINDOWS_FLAG),TRUE)
LIBS       +=-lz
LIBS       +=-lpthread 
LIBS       +=-lws2_32 
else
LIBS       +=-lz$(CROSS_NAME)
endif

SUBDIRS = $(TOP)/core/common $(TOP)/core/varapi $(TOP)/core/if4api $(TOP)/core/zlib

CFLAGS      +=-finline-limit=10000
CFLAGS      +=--param inline-unit-growth=40000
CFLAGS      +=--param large-function-insns=20000
CFLAGS      +=--param large-function-growth=1000

CFLAGS      += -D__DEBUG__
LDFLAGS     += 

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): libs $(OBJECTS)
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) -o $@ $(LIB_PATH) $(LIBS)
endif
	@cp $@ $(TOP)/bin/test

libs: 
	@set -e; for i in $(SUBDIRS); do echo $$i; $(MAKE) -C $$i; done

%.o: %.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c $<

doxygen:
	doxygen Doxyfile

clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@rm -f *.o
	@if [ -f "$(TOP)/bin/test/$(TARGET)" ]; then rm -f $(TOP)/bin/test/$(TARGET); fi

