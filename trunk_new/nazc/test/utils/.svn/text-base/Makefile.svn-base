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

BASE = ../..

include $(BASE)/Shared.rules
include $(BASE)/Makefile.rules

INCLUDES    =$(SYSINCPATH) -I.
INCLUDES   +=-I$(BASE)/include
INCLUDES   +=-I$(BASE)/include/common
INCLUDES   +=-I$(BASE)/include/varapi
INCLUDES   +=-I$(BASE)/include/if4api

LIB_PATH    =$(BASE)/lib

LIBS       +=-lcommon$(CROSS_NAME)
LIBS       +=-lvarapi$(CROSS_NAME)
LIBS       +=-lif4api$(CROSS_NAME)
LIBS       +=-lz$(CROSS_NAME)
LIBS       +=$(CLIBS)

CFLAGS      +=

SOURCES = Variable.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OBJECTS) asyncCall.exe 

asyncCall.exe: asyncCall.o
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(CFLAGS) -I. $(INCLUDES) $(OBJECTS) $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ $+
else
	$(CPP) $(CFLAGS) -I. $(INCLUDES) $(OBJECTS) -o $@ $+ $(LIB_PATH) $(LIBS)
endif

%.o: %.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c $<

clean:
	@rm -f *.o *.exe
	@rm -rf docs

