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

DEBUG			= 0
CROSS_CPU		=
CROSS_OS		=
CROSS_NAME		=
CROSS_COMPILE	= 
SYSINCPATH  	=
SYSLIBPATH  	=
CC 				= $(CROSS_COMPILE)gcc
CPP 			= $(CROSS_COMPILE)g++
LD 				= $(CROSS_COMPILE)ld
AR				= $(CROSS_COMPILE)ar
STRIP			= $(CROSS_COMPILE)strip
ECHO			= /bin/echo

TARGET      	=mibc
INCLUDES    	=-I.
INCLUDES       +=-I../../../include
INCLUDES       +=-I../../../include/varapi
CFLAGS      +=
LDFLAGS     += 

CFLAGS +=-O2

SOURCES = mibc.cpp	\
		parse.cpp	\
		util.cpp

OBJECTS = $(SOURCES:.cpp=.o)
LISTS = $(SOURCES:.cpp=.lst)

all: $(TARGET)
package: $(TARGET)
$(TARGET): $(SOURCES) $(OBJECTS)
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -o $@ 

%.o:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

assemble: $(LISTS)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c -g -Wa,-a,-ad $<  > $@

clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@rm -f *.o *.lst

