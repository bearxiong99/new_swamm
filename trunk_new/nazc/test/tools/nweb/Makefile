###########################################################################
##
###########################################################################

TOP         =../../..

TARGET      =nweb.exe
INCLUDES    =$(SYSINCPATH) -I.

CFLAGS      +=
LDFLAGS     += 

SOURCES = nweb.c
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -Wl,-export-dynamic -Wl,-rpath,./lib -o $@ 
else
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIB_PATH) $(LIBS) -o $@ 
endif
	@cp $@ $(TOP)/bin/test
	@cp nweb.sh $(TOP)/bin/test

%.o: %.c
	$(CPP) $(CFLAGS) $(INCLUDES) -c $<

clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@rm -f *.o *.log
	@if [ -f "$(TOP)/bin/test/$(TARGET)" ]; then rm -f $(TOP)/bin/test/$(TARGET); fi
	@if [ -f "$(TOP)/bin/test/nweb.log" ]; then rm -f $(TOP)/bin/test/nweb.log; fi

