
TOP = ../..
include $(TOP)/Shared.rules
include $(TOP)/Makefile.rules

TARGET      = $(TOP)/bin/gpioctl
LDFLAGS    +=
CFLAGS     += 

INCLUDES    =$(SYSINCPATH) -I.
INCLUDES   +=-I$(TOP)/include
INCLUDES   +=-I$(TOP)/include/common
INCLUDES   +=-I$(TOP)/include/gpio

LIB_PATH	=$(SYSLIBPATH) -L.
LIB_PATH   +=-L$(TOP)/lib

LIBS       +=-lgpio$(CROSS_NAME)
LIBS       +=-lcommon$(CROSS_NAME)
LIBS       +=-lsqlite3
LIBS       +=-lssl
LIBS       +=-lcrypto
LIBS       +=$(CLIBS)

SOURCES =   gpio.c	

OBJECTS =$(SOURCES:.c=.o)
LISTS = $(SOURCES:.cpp=.lst)

all: $(TARGET)
package: $(TARGET)

dev: all
	@cp $(TARGET) /nfs

$(TARGET): $(SOURCES) $(OBJECTS)
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -o $@ 
endif

%.o:	%.c
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

assemble: $(LISTS)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c -g -Wa,-a,-ad $<  > $@

clean:
	@rm -f $(TARGET)
	@rm -f *.o *.lst
	@rm -f cscope.out

