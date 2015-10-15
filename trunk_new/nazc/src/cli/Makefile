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

include ../../Shared.rules
include ../../Makefile.rules

TARGET      =../../bin/cli
INCLUDES    =$(SYSINCPATH) -I.
INCLUDES   +=-I../../include
INCLUDES   +=-I../../include/common
INCLUDES   +=-I../../include/varapi
INCLUDES   +=-I../../include/if4api
INCLUDES   +=-I../../include/codiapi
INCLUDES   +=-I../../include/cli
INCLUDES   +=-I../../include/gdbm
INCLUDES   +=-I../../core/gpio/driver/include
INCLUDES   +=-I../../include/protocol/code

INCLUDES   +=-I../../include/protocol/dlms
INCLUDES   +=-I../../include/protocol/mbus
INCLUDES   +=-I../../include/protocol/kmp
INCLUDES   +=-I../../include/protocol/kukdong


LIB_PATH    =-L../../lib

LIBS       +=-lpdlms$(CROSS_NAME)
LIBS       +=-lpkmp$(CROSS_NAME)
LIBS       +=-lpmbus$(CROSS_NAME)
LIBS       +=-lcode$(CROSS_NAME)

LIBS       +=-lclicore$(CROSS_NAME)
LIBS       +=-lif4api$(CROSS_NAME)
LIBS       +=-lvarapi$(CROSS_NAME)
LIBS       +=-lcommon$(CROSS_NAME)

LIBS       +=-lsqlite3
LIBS       +=-lssl
LIBS       +=-lcrypto
ifeq ($(NZC_TYPE),NZC4)
LIBS       +=-lz
else
LIBS       +=-lz$(CROSS_NAME)
endif
LIBS       +=$(CLIBS)

CFLAGS     += 
LDFLAGS    += 

SOURCES = cli.cpp	\
		Variable.cpp	\
		cmd_table.cpp	\
		cmd_syntax.cpp	\
		cmd_show.cpp	\
		cmd_set.cpp	\
		cmd_default.cpp	\
		cmd_message.cpp	\
		cmd_ctrl.cpp	\
		cmd_util.cpp	\
		cmd_emergency.cpp	\
		cmd_test.cpp

OBJECTS = $(SOURCES:.cpp=.o)
LISTS = $(SOURCES:.cpp=.lst)

all: $(TARGET)
package: $(TARGET)
$(TARGET): $(SOURCES) $(OBJECTS)
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -o $@ 
endif

dev: all
	@cp $(TARGET) /nfs

%.o:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

assemble: $(LISTS)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c -g -Wa,-a,-ad $<  > $@

var:
	./mibc -nohandler variable.oid > var	

clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@rm -f *.o *.lst

