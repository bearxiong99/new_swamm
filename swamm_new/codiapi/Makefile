
include Shared.rules
include Makefile.rules

MAKE 	= make
SUBDIRS = src samples
TESTDIRS = src

all: libs
	@set -e; for i in $(SUBDIRS); do echo $$i; $(MAKE) -C $$i URBAN=1; done
package: libs
	@set -e; for i in $(SUBDIRS); do echo $$i; $(MAKE) -C $$i package URBAN=1; done

test:
	@set -e; for i in $(TESTDIRS); do echo $$i; $(MAKE) -C $$i test; done

libs: 
	@if [ ! -e "../nazc/lib/libz$(CROSS_NAME).a" ]; then $(MAKE) -C ../nazc/core/zlib ; fi
	@if [ ! -h "lib/libz$(CROSS_NAME).a" ]; then ln -s ../../nazc/lib/libz$(CROSS_NAME).a lib/libz$(CROSS_NAME).a; fi
	@if [ ! -e "../nazc/lib/libsqlite3.$(LIB_EXT)" ]; then $(MAKE) -C ../nazc/third ; fi
	@if [ ! -h "lib/libsqlite3.$(LIB_EXT)" ]; then ln -s ../../nazc/lib/libsqlite3.$(LIB_EXT) lib/libsqlite3.$(LIB_EXT); fi
	@if [ ! -e "../nazc/lib/libcrypto.$(LIB_EXT)" ]; then $(MAKE) -C ../nazc/third ; fi
	@if [ ! -h "lib/libcrypto.$(LIB_EXT)" ]; then ln -s ../../nazc/lib/libcrypto.$(LIB_EXT) lib/libcrypto.$(LIB_EXT); fi
	@if [ ! -e "../nazc/lib/libssl.$(LIB_EXT)" ]; then $(MAKE) -C ../nazc/third ; fi
	@if [ ! -h "lib/libssl.$(LIB_EXT)" ]; then ln -s ../../nazc/lib/libssl.$(LIB_EXT) lib/libssl.$(LIB_EXT); fi
	@if [ ! -e "../nazc/lib/libcommon$(CROSS_NAME).$(LIB_EXT)" ]; then $(MAKE) -C ../nazc/core/common ; fi
	@if [ ! -h "lib/libcommon$(CROSS_NAME).$(LIB_EXT)" ]; then ln -s ../../nazc/lib/libcommon$(CROSS_NAME).$(LIB_EXT) lib/libcommon$(CROSS_NAME).$(LIB_EXT); fi
	@if [ ! -e "../nazc/lib/libmcommon$(CROSS_NAME).$(LIB_EXT)" ]; then $(MAKE) -C ../nazc/core/common ; fi
	@if [ ! -h "lib/libmcommon$(CROSS_NAME).$(LIB_EXT)" ]; then ln -s ../../nazc/lib/libmcommon$(CROSS_NAME).$(LIB_EXT) lib/libmcommon$(CROSS_NAME).$(LIB_EXT); fi

lib/libmcommon$(CROSS_NAME).$(LIB_EXT):
	@if [ ! -e "../nazc/lib/libz$(CROSS_NAME).a" ]; then $(MAKE) -C ../nazc/core/zlib ; fi

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done
	@if [ -e "lib/libz$(CROSS_NAME).a" ]; then rm -f lib/libz$(CROSS_NAME).a; fi
	@if [ -e "lib/libcommon$(CROSS_NAME).$(LIB_EXT)" ]; then rm -f lib/libcommon$(CROSS_NAME).$(LIB_EXT); fi
	@if [ -e "lib/libmcommon$(CROSS_NAME).$(LIB_EXT)" ]; then rm -f lib/libmcommon$(CROSS_NAME).$(LIB_EXT); fi
	@if [ -e "lib/libcodiapi$(CROSS_NAME).$(LIB_EXT)" ]; then rm -f lib/libcodiapi$(CROSS_NAME).$(LIB_EXT); fi
	@if [ -e "lib/libsqlite3.$(LIB_EXT)" ]; then rm -f lib/libsqlite3.$(LIB_EXT); fi
	@if [ -e "lib/libcrypto.$(LIB_EXT)" ]; then rm -f lib/libcrypto.$(LIB_EXT); fi
	@if [ -e "lib/libssl.$(LIB_EXT)" ]; then rm -f lib/libssl.$(LIB_EXT); fi

