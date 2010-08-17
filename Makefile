CC         	:= gcc
CXX         := g++

DEFINES		:=
INCLUDES	:=
LIBRARIES	:=
COMMONFLAGS	:= -Wall -W $(DEFINES) $(INCLUDES)

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

CFLAGS		:= $(COMMONFLAGS) -std=c99
CXXFLAGS	:= $(COMMONFLAGS)

TARGET		:= bowling
OBJS		:= generate_rolls.o bowling.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBRARIES)

clean:
	rm -f $(OBJS) $(TARGET)
