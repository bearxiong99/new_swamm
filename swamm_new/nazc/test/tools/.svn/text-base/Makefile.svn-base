###########################################################################
##
###########################################################################


SUBDIRS = nweb

all:
	@set -e; for i in $(SUBDIRS); do echo $$i; $(MAKE) -C $$i; done

package: all

test: all

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done

