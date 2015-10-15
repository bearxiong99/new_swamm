###########################################################################
##
## MDK (AIMIR MCU Development Kit) v1.0
##
## Copyright (c) 2005 NURI-Telecom Co., Ltd. 
## All rights reserved. 
##
## Update History
## 	2005/11/03 casir@paran.com		Lee youngjin
##
###########################################################################

MAKE 	= make
SRCDIRS = core third src 
BINDIRS = bin
SUBDIRS = $(SRCDIRS) $(BINDIRS)
TESTDIRS = core src ../codiapi test

have_shared_rules := $(wildcard Shared.rules)
have_makefile_rules := $(wildcard Makefile.rules)

ifeq ($(strip $(have_shared_rules)),Shared.rules)
include Shared.rules
endif
ifeq ($(strip $(have_makefile_rules)),Makefile.rules)
include Makefile.rules
endif

all: genlink codiapi
ifeq ($(strip $(have_shared_rules)),)
	@echo "Shared.rules file not found."
else
	@set -e; for i in $(SUBDIRS); do echo $$i; $(MAKE) -C $$i; done
endif

package: genlink codiapi
ifeq ($(strip $(have_shared_rules)),)
	@echo "Shared.rules file not found."
else
	@set -e; for i in $(SUBDIRS); do echo $$i; $(MAKE) -C $$i package; done
endif

pkgwfw: genlink codiapi
ifeq ($(strip $(have_shared_rules)),)
	@echo "Shared.rules file not found."
else
	@set -e; for i in $(SRCDIRS); do echo $$i; $(MAKE) -C $$i package; done
	@set -e; for i in $(BINDIRS); do echo $$i; $(MAKE) -C $$i pkgwfw; done
endif

test: fixture 
	@echo "build test"
	@set -e; for i in $(TESTDIRS); do echo $$i; $(MAKE) -C $$i test; done

testrun: test
	@(cd bin/test; ./runUnitTests.sh)

fixture: 
	@echo "fixture"
	@$(MAKE) -C bin fixture

genlink: 
	@if [ ! -h "lib/libcodiapi$(CROSS_NAME).$(LIB_EXT)" ]; then ln -s ../../codiapi/lib/libcodiapi$(CROSS_NAME).$(LIB_EXT) lib/libcodiapi$(CROSS_NAME).$(LIB_EXT); fi

codiapi: 
	@if [ ! -e "../codiapi/lib/libcodiapi$(CROSS_NAME).$(LIB_EXT)" ]; then $(MAKE) -C ../codiapi ; fi

assemble: 
	@set -e; for i in $(SUBDIRS); do echo $$i; $(MAKE) -C $$i assemble; done

tag: 
	@echo "generate tags"
	@ctags -R . ../codiapi 2> /dev/null

cscope: 
	@echo "generate cscope"
	@cscope -Rb -s . -s ../codiapi 2> /dev/null

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done
	@if [ -e "lib/libclicore$(CROSS_NAME).$(LIB_EXT)" ]; then rm -f lib/libclicore$(CROSS_NAME).$(LIB_EXT); fi
	@if [ -e "lib/libgpio$(CROSS_NAME).$(LIB_EXT)" ]; then rm -f lib/libgpio$(CROSS_NAME).$(LIB_EXT); fi
	@if [ -e "lib/libz$(CROSS_NAME).$(LIB_EXT)" ]; then rm -f lib/libz$(CROSS_NAME).$(LIB_EXT); fi
	@if [ -e "lib/libcommon$(CROSS_NAME).$(LIB_EXT)" ]; then rm -f lib/libcommon$(CROSS_NAME).$(LIB_EXT); fi
	@if [ -e "lib/libif4api$(CROSS_NAME).$(LIB_EXT)" ]; then rm -f lib/libif4api$(CROSS_NAME).$(LIB_EXT); fi
	@if [ -e "lib/libvarapi$(CROSS_NAME).$(LIB_EXT)" ]; then rm -f lib/libvarapi$(CROSS_NAME).$(LIB_EXT); fi
	@if [ -e "lib/libcodiapi$(CROSS_NAME).$(LIB_EXT)" ]; then rm -f lib/libcodiapi$(CROSS_NAME).$(LIB_EXT); fi

cleanall: clean
	@$(MAKE) -C test clean 
	@$(MAKE) -C ../codiapi clean 
