CC=gcc
SRC_DIR=$(shell pwd)

GENERIC_INCLUDES+=					\
	$(SRC_DIR) 

GENERIC_BINARIES_COMMON_SOURCE_FILES=			\
	ImgProcTest.c	\
   UI/gui.c \
   ImgProc/ImgProc.c \
   UI/gamepad.c \
   ttiofiles/ttiofiles.c      
    
    
GENERIC_INCLUDES:=$(addprefix -I,$(GENERIC_INCLUDES))
GENERIC_INCLUDES += `pkg-config --cflags opencv glib-2.0 gtk+-2.0` 
    
GENERIC_LIBS= -lgtk-x11-2.0 -lrt `pkg-config --libs opencv gthread-2.0 gtk+-2.0 glib-2.0`  -lgdk-x11-2.0 -lgdk_pixbuf-2.0 -lgobject-2.0 -lm -lcairo



#export GENERIC_LIBS
#export GENERIC_INCLUDES
#export GENERIC_BINARIES_COMMON_SOURCE_FILES

TARGET=noArdroneTest

all: $(TARGET)

$(TARGET):
	$(CC) $(GENERIC_INCLUDES) $(GENERIC_BINARIES_COMMON_SOURCE_FILES) $(GENERIC_LIBS) -o $(TARGET) 
