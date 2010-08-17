CXX         := 	g++
CC         	:=	gcc
LINKER     	:= 	g++ -fPIC
LIBRARIES	:=

DEFINES		:=
COMMONFLAGS	:= $(DEFINES)

ifeq ($(use_freeimage),1)
	LIBRARIES	+= -lfreeimage
	DEFINES 	+= -DUSE_FREEIMAGE
endif

ifeq ($(dbg),1)
	COMMONFLAGS += -g
else
	COMMONFLAGS += -O3
endif

ifeq ($(prof),1)
	COMMONFLAGS += -pg
endif
ifeq ($(omp),1)
	COMMONFLAGS += -fopenmp
endif

CXXFLAGS	:= -Wall -W $(INCPATH) $(COMMONFLAGS)

TARGET		:= bowling
OBJS		:= read_write_rolls.o bowling.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)


clean:
	rm -f $(OBJS) $(OBJS_1) $(TARGET) $(TARGET_1)
