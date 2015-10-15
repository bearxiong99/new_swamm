##################################################################
#
#  NURI DAU Board Device Make File 
#
##################################################################
include ../../../../Shared.rules
include ../../../../Makefile.rules

KERNELDIR 		= ../../../../../nzc-kernel
DEV_INCLUDEDIR 	= -I$(KERNELDIR)/include  -I./ -I../include

CC      = armv5l-linux-gcc
CXX     = armv5l-linux-g++
AR      = armv5l-linux-ar
LD      = armv5l-linux-ld
NM      = armv5l-linux-nm
STRIP   = armv5l-linux-strip
OBJCOPY = armv5l-linux-objcopy

include $(KERNELDIR)/.config

CFLAGS +=  -D__KERNEL__ -DMODULE $(DEV_INCLUDEDIR) $(DEBFLAGS)

TARGET 	= ../../../../bin/driver/gpio_dev
OBJS	= $(TARGET).o
    
SRCS   = gpio.c 

all: $(TARGET).o  
package: $(TARGET).o  
    
$(TARGET).o: $(SRCS:.c=.o)
	$(LD) $(LDFLAGS) -r $^ -o $@

clean:
	rm -f *.o *~ core .depend

dep :        
	gccmakedep $(DEV_INCLUDEDIR) $(SRCS) 

assemble: $(SRCS:.c=.lst)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(DEV_INCLUDEDIR) -c -g -Wa,-a,-ad $<  > $@


