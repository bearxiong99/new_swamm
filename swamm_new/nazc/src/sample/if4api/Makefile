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

include ../../../Shared.rules
include ../../../Makefile.rules

INCLUDES    =$(SYSINCPATH) -I.
INCLUDES   +=-I../../../include
INCLUDES   +=-I../../../include/common
INCLUDES   +=-I../../../include/varapi
INCLUDES   +=-I../../../include/if4api

LIB_PATH   +=-L../../../lib

LIBS       +=-lcommon$(CROSS_NAME)
LIBS       +=-lvarapi$(CROSS_NAME)
LIBS       +=-lif4api$(CROSS_NAME)
LIBS       +=-lz$(CROSS_NAME)
ifeq ($(DISABLE_SSL),)
LIBS       +=-lssl
LIBS       +=-lcrypto
endif
ifeq ($(DISABLE_SQLITE3),)
LIBS       +=-lsqlite3
endif
ifeq ($(WINDOWS_FLAG),TRUE)
LIBS       +=-lws2_32
LIBS       +=-lpthread      
endif
LIBS       +=$(CLIBS)

CFLAGS      +=
LDFLAGS     += 

SOURCES = Variable.cpp
OBJECTS = $(SOURCES:.cpp=.o)
LISTS = $(SOURCES:.cpp=.lst)

all: $(OBJECTS) client.exe server.exe
package: $(OBJECTS) client.exe server.exe

client.exe: client.o
	$(CPP) $(LDFLAGS) -I. $(INCLUDES) $(LIB_PATH) $(DUMMY_LIB_PATH) $(OBJECTS) $(LIBS) -o $@ $+

server.exe: server.o if4CallbackHandler.o
	$(CPP) $(LDFLAGS) -I. $(INCLUDES) $(LIB_PATH) $(OBJECTS) $(LIBS) -o $@ $+

doxygen:
	doxygen Doxyfile

%.o: %.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c $<

assemble: $(LISTS)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c -g -Wa,-a,-ad $<  > $@

clean:
	@rm -f *.o *.exe *.lst
	@rm -rf docs

#inserted by sungyeung
#"tap" charactoer  is always located front of @sudo
dev: all
	@sudo cp $(TARGET) /home/gloscom/nfs/new_sweden_151013/

	
	