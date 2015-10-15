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

TOP = ../..

include $(TOP)/Shared.rules
include $(TOP)/Makefile.rules

DEST        =$(TOP)/bin
TARGET      =$(TOP)/bin/update
INCLUDES    =-I.
INCLUDES   +=-I$(TOP)/include
INCLUDES   +=-I$(TOP)/include/common

ifeq ($(ARM),1)
INCLUDES   +=-I$(TOP)/include/gpio
endif

LIB_PATH    =-L$(TOP)/lib
LIBS       +=-lcodiapi$(CROSS_NAME)
LIBS       +=-lcommon$(CROSS_NAME)
ifeq ($(DISABLE_SSL),)
LIBS       +=-lssl
LIBS       +=-lcrypto
endif
ifeq ($(DISABLE_SQLITE3),)
LIBS       +=-lsqlite3
endif
ifeq ($(WINDOWS_FLAG),TRUE)
LIBS       +=-lws2_32
endif
LIBS       +=$(CLIBS)

CFLAGS     +=-Wall
LDFLAGS    += 

ifeq ($(ARM),1)
CFLAGS +=-DEMBEDDED_LINUX
endif

ifeq ($(DEBUG),1)
CFLAGS +=-g -O2 -DDEBUG 
else
CFLAGS +=-O2 
endif

SOURCES = update.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET) hostfiles
package: $(TARGET)
$(TARGET): $(SOURCES) $(OBJECTS)
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -o $@ 
endif

hostfiles: mingw
mingw: $(DEST)/pthreadGC2.dll
$(DEST)/pthreadGC2.dll: 
ifeq ($(WINDOWS_FLAG),TRUE)
	cp $(TOP)/../nazc/hosts/mingw/pthreadGC2.dll $(DEST)
endif

%.o:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c $<

clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@rm -f *.o

