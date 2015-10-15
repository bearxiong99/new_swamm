###########################################################################
##
## Copyright (c) 2011 NURI-Telecom Co., Ltd. 
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
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
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

include ../Shared.rules

SUBDIRS = smspdu

all: thirdLibBuild
	@set -e; for i in $(SUBDIRS); do echo $$i; $(MAKE) -C $$i; done

package: thirdLibBuild
	@set -e; for i in $(SUBDIRS); do echo $$i; $(MAKE) -C $$i; done

thirdLibBuild: openssl sqlite3

openssl: ../lib/libcrypto.$(LIB_EXT) ../lib/libssl.$(LIB_EXT)

../lib/libcrypto.$(LIB_EXT): 
ifeq ($(DISABLE_SSL),TRUE)
	@echo "skip SSL..."
else
ifneq ($(NZC_TYPE),NZC4)
	@$(MAKE) -C openssl-1.0.0a
endif
endif

../lib/libssl.$(LIB_EXT): 
ifeq ($(DISABLE_SSL),TRUE)
	@echo "skip SSL..."
else
ifneq ($(NZC_TYPE),NZC4)
	@$(MAKE) -C openssl-1.0.0a
endif
endif

sqlite3: ../lib/libsqlite3.$(LIB_EXT)
../lib/libsqlite3.$(LIB_EXT):
ifeq ($(DISABLE_SQLITE3),TRUE)
	@echo "skip SQLITE3..."
else
ifneq ($(NZC_TYPE),NZC4)
	@$(MAKE) -C sqlite-autoconf-3070500 DESTDIR=../../lib
endif
endif

test:
	@echo ""

assemble:
	@echo ""

clean:
	@echo ""
