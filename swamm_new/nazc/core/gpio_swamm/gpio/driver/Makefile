#######################################################################
#
#  샘플 디비이스 시험용  메이크 화일 
#  이 메이크 화일은 등록된 디렉토리에 있는 모든 것을 make 수행한다.
#
#######################################################################

#######################################################################
#
# 이 화일에 선언된 모든 변수를 서브 메이크 화일에서 사용할수 있게 한다.
#
#######################################################################

.EXPORT_ALL_VARIABLES:

#######################################################################
#
# 컴파일 할 대상이 있는 디렉토리를 적은다. 
#
#######################################################################

DIRS = device 
TOPDIR	:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

CC      = arm-xscale-linux-gnu-gcc
CXX     = arm-xscale-linux-gnu-g++
AR      = arm-xscale-linux-gnu-ar
LD      = arm-xscale-linux-gnu-ld
NM      = arm-xscale-linux-gnu-nm
STRIP   = arm-xscale-linux-gnu-strip
OBJCOPY = arm-xscale-linux-gnu-objcopy

#CC      = armv5l-linux-gcc
#CXX     = armv5l-linux-g++
#AR      = armv5l-linux-ar
#LD      = armv5l-linux-ld
#NM      = armv5l-linux-nm
#STRIP   = armv5l-linux-strip
#OBJCOPY = armv5l-linux-objcopy
CP 		= cp
MV 		= mv

all:
	for i in $(DIRS) ; do make -C $$i || exit $? ; done

package: 
	for i in $(DIRS) ; do make -C $$i package || exit $? ; done

clean:
	for i in $(DIRS) ; do make -C $$i clean; done

dep:
	for i in $(DIRS) ; do make -C $$i dep; done

assemble:
	for i in $(DIRS) ; do make -C $$i assemble; done

