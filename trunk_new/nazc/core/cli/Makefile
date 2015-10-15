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

LIB_NAME    =libclicore$(CROSS_NAME).$(LIB_EXT)
TARGET      =$(TOP)/lib/$(LIB_NAME)
INCLUDES    =$(SYSINCPATH) -I. -I$(TOP)/include
INCLUDES   +=-I$(TOP)/include/common
INCLUDES   +=-I$(TOP)/include/cli
INCLUDES   +=-I$(TOP)/include/varapi
INCLUDES   +=-I$(TOP)/include/if4api
LDFLAGS    +=
CFLAGS     +=-c

LIB_PATH    =-L$(TOP)/lib
SHARED_LIB_PATH    =$(TOP)/bin/lib

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)
LISTS = $(SOURCES:.cpp=.lst)

all: $(TARGET)
package: $(TARGET)

dev: all
	@cp $(TARGET) /nfs

$(TARGET): $(SOURCES) $(OBJECTS)
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) -shared -Wl,-soname,$(LIB_NAME) $(OBJECTS) -o $(TARGET)
	cp -f $(TARGET) $(SHARED_LIB_PATH)/$(LIB_NAME)
else
	$(AR) rc $(TARGET) $(OBJECTS)
endif

%.o:	%.cpp
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(CFLAGS) $(INCLUDES) -fPIC -c $< -o $@
else
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@
endif

assemble: $(LISTS)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c -g -Wa,-a,-ad $<  > $@

clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(SHARED_LIB_PATH)/$(LIB_NAME)" ]; then rm -f $(SHARED_LIB_PATH)/$(TARGET); fi
	@rm -f *.o *.lst

