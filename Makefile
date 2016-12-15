SRIPATH = /home/alexander/Desktop/srilm-1.7.2
MACHINE_TYPE = i686-m64

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc -lz 
	$(CXX) $(LDFLAGS) -o $@ $^ -pthread -fopenmp

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) $(OBJ) $(TARGET)