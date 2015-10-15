###########################################################################
## Make compress file
###########################################################################

include ../Shared.rules
include ../Makefile.rules

PRODUCT=NZC

GSM_INDOOR  =I211
GSM_OUTDOOR =O312
NOBATT_INDOOR  =I211MB
CDMA_OUTDOOR =O201
ifeq ($(NZC_TYPE),NZC2)
VERSION	=3.1
else
VERSION	=3.0
endif
PROJECT	=AMM

REVISIONFILE=revision.info
ifeq ($(NO_BATTERY),)
PKGCHECKSUM=verification/$(NZC_TYPE)/pkg_checksum.info
else
PKGCHECKSUM=verification/NZCMB/pkg_checksum.info
endif

BSDIFF=`which bsdiff`
FIXTURE=test/fixture/fwImages/Concentrator

INSTALLSCRIPT = relaunching verify.script
LIBRARYFILES = lib

EXECUTABLE = launcher        \
		sysmon			\
		agent			\
		cli				\
		upload			\
		mserver			\
		mterm			\
		send			\
		dhcpcd			\
		bsdiff			\
		bspatch			\
		pmon

MODULES = $(wildcard driver/*.o)
MODULES2 = $(wildcard driver2/*.o)
NONEXECUTABLE = install	\
        $(MODULES)      \
        $(MODULES2)     \
		startup			\
		pkginstall		\
		timesync        \
        ftp.script      \
        dump.script     \
		sensor.type	    \
		dhcpd           \
		ppp				\
		zoneinfo	    \
		script	        \
		revision.info   \
		application.info    \
		patch.sh        \
		calendar.ini	\
        cert.pem        \
        server.pem

EXCEPTCHECKSUM = checksum.info  \
		mcumodel        \
		mcusystype 

FWPKG = fwPkg.tar.gz

SOURCE = $(INSTALLSCRIPT) $(LIBRARYFILES) $(EXECUTABLE) $(NONEXECUTABLE) $(EXCEPTCHECKSUM)

all: clean copydriver revision checksum tarball

package: clean copydriver revision strip checksum tarball script
pkgwfw: clean copydriver revision strip checksum tarballfw scriptfw

create_dir: 
		@if [ ! -d verification ]; then mkdir verification 2> /dev/null; fi
ifeq ($(NO_BATTERY),)
		@if [ ! -d verification/$(NZC_TYPE) ]; then mkdir verification/$(NZC_TYPE) 2> /dev/null; fi
else
		@if [ ! -d verification/NZCMB ]; then mkdir verification/NZCMB 2> /dev/null; fi
endif

tarball: create_dir
		@cat /dev/null > $(PKGCHECKSUM)
ifeq ($(CROSS_NAME),)
		@echo " + Skip Tarball Files"
else
		@echo " + Create Tarball Files"
ifeq ($(NO_BATTERY),)
		@echo "OUTDOOR" > mcusystype
		@echo "$(GSM_OUTDOOR)" > mcumodel
		@tar zcf $(PRODUCT)_$(GSM_OUTDOOR)-v$(VERSION)-`cat revision.info`.tar.gz $(SOURCE) --exclude=".svn"
		@md5sum $(PRODUCT)_$(GSM_OUTDOOR)-v$(VERSION)-`cat revision.info`.tar.gz | sed -e 's/\*//g' >> $(PKGCHECKSUM)
ifeq ($(NZC_TYPE),NZC2)
		@echo "$(CDMA_OUTDOOR)" > mcumodel
		@tar zcf $(PRODUCT)_$(CDMA_OUTDOOR)-v$(VERSION)-`cat revision.info`.tar.gz $(SOURCE) --exclude=".svn"
		@md5sum $(PRODUCT)_$(CDMA_OUTDOOR)-v$(VERSION)-`cat revision.info`.tar.gz | sed -e 's/\*//g' >> $(PKGCHECKSUM)
endif
		@echo "INDOOR" > mcusystype
		@echo "$(GSM_INDOOR)" > mcumodel
		@tar zcf $(PRODUCT)_$(GSM_INDOOR)-v$(VERSION)-`cat revision.info`.tar.gz $(SOURCE) --exclude=".svn"
		@md5sum $(PRODUCT)_$(GSM_INDOOR)-v$(VERSION)-`cat revision.info`.tar.gz | sed -e 's/\*//g' >> $(PKGCHECKSUM)
		@rm -f mcusystype 
else
		@echo "INDOOR" > mcusystype
		@echo "$(NOBATT_INDOOR)" > mcumodel
		@tar zcf $(PRODUCT)_$(NOBATT_INDOOR)-v$(VERSION)-`cat revision.info`.tar.gz $(SOURCE) --exclude=".svn"
		@md5sum $(PRODUCT)_$(NOBATT_INDOOR)-v$(VERSION)-`cat revision.info`.tar.gz | sed -e 's/\*//g' >> $(PKGCHECKSUM)
		@rm -f mcusystype 
endif
endif

tarballfw: 
ifeq ($(CROSS_NAME),)
		@echo " + Skip Tarball Files"
else
		@echo " + Create Tarball Files"
ifeq ($(NO_BATTERY),)
		@echo "OUTDOOR" > mcusystype
		@echo "$(GSM_OUTDOOR)" > mcumodel
		@tar zcf $(PRODUCT)_$(GSM_OUTDOOR)-v$(VERSION)-`cat revision.info`-wFw.tar.gz $(SOURCE) $(FWPKG) --exclude=".svn"
ifeq ($(NZC_TYPE),NZC2)
		@echo "$(CDMA_OUTDOOR)" > mcumodel
		@tar zcf $(PRODUCT)_$(CDMA_OUTDOOR)-v$(VERSION)-`cat revision.info`-wFw.tar.gz $(SOURCE) $(FWPKG) --exclude=".svn"
endif
		@echo "INDOOR" > mcusystype
		@echo "$(GSM_INDOOR)" > mcumodel
		@tar zcf $(PRODUCT)_$(GSM_INDOOR)-v$(VERSION)-`cat revision.info`-wFw.tar.gz $(SOURCE) $(FWPKG) --exclude=".svn"
		@rm -f mcusystype 
else
		@echo "INDOOR" > mcusystype
		@echo "$(NOBATT_INDOOR)" > mcumodel
		@tar zcf $(PRODUCT)_$(NOBATT_INDOOR)-v$(VERSION)-`cat revision.info`.tar.gz $(SOURCE) --exclude=".svn"
		@rm -f mcusystype 
endif
endif

fixture: 
ifeq ($(CROSS_NAME),)
		@echo " + Skip Fixture Files"
else
		@echo " + Create Fixture Files"
ifeq ($(NO_BATTERY),)
		@cp -f $(PRODUCT)_$(GSM_OUTDOOR)-v$(VERSION)-`cat revision.info`.tar.gz $(FIXTURE)/$(PRODUCT)_$(GSM_OUTDOOR)-v$(VERSION).tar.gz
ifeq ($(NZC_TYPE),NZC2)
		@cp -f $(PRODUCT)_$(CDMA_OUTDOOR)-v$(VERSION)-`cat revision.info`.tar.gz $(FIXTURE)/$(PRODUCT)_$(CDMA_OUTDOOR)-v$(VERSION).tar.gz
endif
		@cp -f $(PRODUCT)_$(GSM_INDOOR)-v$(VERSION)-`cat revision.info`.tar.gz $(FIXTURE)/$(PRODUCT)_$(GSM_INDOOR)-v$(VERSION).tar.gz
else
		@cp -f $(PRODUCT)_$(NOBATT_INDOOR)-v$(VERSION)-`cat revision.info`.tar.gz $(FIXTURE)/$(PRODUCT)_$(NOBATT_INDOOR)-v$(VERSION).tar.gz
endif
endif

copydriver: 
		@echo " + Update Driver Files"
ifeq ($(NZC_TYPE),NZC2)
		@cp driver/gpio_dev.nzc2 driver/gpio_dev.o
else
		@cp driver/gpio_dev.nzc1 driver/gpio_dev.o
endif

revision :
		@echo " + Create Revision Infomation"
		@svnversion -c .. |  sed -e 's/\([0-9]*:\)\([0-9]\+[MS]*\).*/\2/' > revision.info
		@echo "   Source Tree : `svn info .. | grep URL | sed -e 's/.*http.*mcu\/\(.*\)\/[0-9a-zA-Z]*/\1/'`" > application.info
		@echo "      Revision : `cat revision.info`" >> application.info
		@(LANG=C;echo "    Build Date : `date +'%c %:::z'`" >> application.info)
		@echo " Build Machine : `uname -a`" >> application.info
		@echo "      Compiler : `$(CPP) --version | head -1`" >> application.info
		@echo "Compile Option : $(CFLAGS)" | sed -e 's/-W[a-zA-Z0-9\-]*//g' | sed -e 's/-D__GE.*} //g' \
            | sed -e 's/-D__SUPPORT_NZC.__//g' | sed -e 's/-D__PATCH_.*__ //g' | sed -e 's/-O.//g' \
            | sed -e 's/\(-[a-zA-Z0-9_-]*\)[ ]*/                 \1\n/g' \
            | sed -e 's/Compile Option :[ ]*/Compile Option : /' \
            | sed -e '/^$$/d' \
                >> application.info

strip: 
		@echo " + Strip"
		@$(STRIP) $(EXECUTABLE)
		@$(STRIP) lib/*

script: revision 
		@echo " + Make Install Script"
ifeq ($(NO_BATTERY),)
		@./mkInstallScript.sh $(PRODUCT)_$(GSM_OUTDOOR)-v$(VERSION)-`cat revision.info`.tar.gz > factoryInstall_$(GSM_OUTDOOR)
		@./mkInstallScript.sh $(PRODUCT)_$(GSM_INDOOR)-v$(VERSION)-`cat revision.info`.tar.gz > factoryInstall_$(GSM_INDOOR)
ifeq ($(NZC_TYPE),NZC2)
		@./mkInstallScript.sh $(PRODUCT)_$(CDMA_OUTDOOR)-v$(VERSION)-`cat revision.info`.tar.gz > factoryInstall_$(CDMA_OUTDOOR)
endif
else
		@./mkInstallScript.sh $(PRODUCT)_$(NOBATT_INDOOR)-v$(VERSION)-`cat revision.info`.tar.gz > factoryInstall_$(NOBATT_INDOOR)
endif

scriptfw: revision 
		@echo " + Make Install Script"
ifeq ($(NO_BATTERY),)
		@./mkInstallScript.sh $(PRODUCT)_$(GSM_OUTDOOR)-v$(VERSION)-`cat revision.info`-wFw.tar.gz > factoryInstall_$(GSM_OUTDOOR)
		@./mkInstallScript.sh $(PRODUCT)_$(GSM_INDOOR)-v$(VERSION)-`cat revision.info`-wFw.tar.gz > factoryInstall_$(GSM_INDOOR)
ifeq ($(NZC_TYPE),NZC2)
		@./mkInstallScript.sh $(PRODUCT)_$(CDMA_OUTDOOR)-v$(VERSION)-`cat revision.info`-wFw.tar.gz > factoryInstall_$(CDMA_OUTDOOR)
endif
else
		@./mkInstallScript.sh $(PRODUCT)_$(NOBATT_INDOOR)-v$(VERSION)-`cat revision.info`-wFw.tar.gz > factoryInstall_$(NOBATT_INDOOR)
endif

clean:
		@echo " + Clean"
		@if [ -f "driver/gpio_dev.o" ]; then rm -f driver/gpio_dev.o; fi
		@rm -f checksum.*
		@rm -rf verification
		@rm -f $(PRODUCT)_*.tar.gz
		@rm -f test/test*.xml
		@rm -f test/fixture/fwImages/Concentrator/$(PRODUCT)_*
		@rm -f factoryInstall_*
		 	
checksum: create_dir
ifeq ($(CROSS_NAME),)
		@echo " + Skip CheckSum Infomation"
else
		@echo " + Create CheckSum Infomation"
		@echo $(INSTALLSCRIPT) $(LIBRARYFILES) $(EXECUTABLE) $(NONEXECUTABLE) | ./mkChecksum.sh > checksum.info
ifeq ($(NO_BATTERY),)
		@cp checksum.info verification/$(NZC_TYPE)
else
		@cp checksum.info verification/NZCMB
endif
endif

