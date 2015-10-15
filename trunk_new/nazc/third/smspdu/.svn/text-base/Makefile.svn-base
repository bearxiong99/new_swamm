#
# Makefile
#
# https://code.google.com/p/smspdu-cpp
#

TOP = ../..

include $(TOP)/Shared.rules
include $(TOP)/Makefile.rules

LIB_NAME    =libsms$(CROSS_NAME).$(LIB_EXT)
TARGET      =$(TOP)/lib/$(LIB_NAME)
SHARED_LIB_PATH    =$(TOP)/bin/lib

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEP =	$(wildcard include/*.h)

INCLUDES   +=-Iinclude

all: $(TARGET)
package: $(TARGET)

$(TARGET): $(SOURCES) $(DEP) $(OBJECTS)
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(LDFLAGS) -shared -Wl,-soname,$(LIB_NAME) $(OBJECTS) -o $(TARGET)
	cp -f $(TARGET) $(SHARED_LIB_PATH)/$(LIB_NAME)
else
	$(AR) rc $(TARGET) $(OBJECTS)
endif

dev: all
	@cp $(TARGET) /nfs

%.o:	%.cpp
ifeq ($(strip $(LIB_EXT)),so)
	$(CPP) $(CFLAGS) $(INCLUDES) -fPIC -c $< -o $@
else
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@
endif


clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(SHARED_LIB_PATH)/$(TARGET)" ]; then rm -f $(SHARED_LIB_PATH)/$(TARGET); fi
	@rm -f $(OBJECTS)
	

