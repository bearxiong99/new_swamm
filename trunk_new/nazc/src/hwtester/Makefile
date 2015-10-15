
TOP = ../..
include $(TOP)/Shared.rules
include $(TOP)/Makefile.rules

TARGET      = hwtest_v13
LDFLAGS    +=
CFLAGS     += 

INCLUDES    =$(SYSINCPATH) -I.
INCLUDES   +=-I$(TOP)/include
INCLUDES   +=-I$(TOP)/include/codiapi
INCLUDES   +=-I$(TOP)/include/common
INCLUDES   +=-I$(TOP)/include/gpio

LIB_PATH	=$(SYSLIBPATH) -L.
LIB_PATH   +=-L$(TOP)/lib

LIBS       +=-lcodiapi$(CROSS_NAME)
LIBS       +=-lcommon$(CROSS_NAME)
LIBS       +=-lgpio$(CROSS_NAME)
LIBS       +=-lsqlite3
LIBS       +=-lssl
LIBS       +=-lcrypto
LIBS       +=$(CLIBS)

SOURCES =   main.c	\
            event.c \
            kbhit.c \
            qc.c    \
            gpio.c

OBJECTS =$(SOURCES:.c=.o)
LISTS = $(SOURCES:.cpp=.lst)

all: $(TARGET)
package: $(TARGET)

$(TARGET): $(SOURCES) $(OBJECTS)
ifeq ($(NZC_TYPE),NZC4)
	@echo "TODO : Not Working"
else
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -o $@ 
endif
	@cp  $(TARGET) ../../bin/hwtest
endif

%.o:	%.c
ifeq ($(NZC_TYPE),NZC4)
	
else
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@
endif

assemble: $(LISTS)
%.lst:	%.cpp
	$(CPP) $(CFLAGS) $(INCLUDES) -c -g -Wa,-a,-ad $<  > $@

clean:
	@rm -f $(TARGET)
	@rm -f *.o *.lst
	@rm -f cscope.out

