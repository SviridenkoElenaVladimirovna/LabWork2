PROJECT = game

CXX = g++
CXXFLAGS = -I. -ICards -ICore -ICore/events -IData -IPlayers -ISystems -IUI
LDIR = ../lib
LIBS = -lm

SRC_DIRS = Cards Core Core/events Data Players Systems UI
SRCS = main.cpp $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean cleanall

all: $(PROJECT)

$(PROJECT): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(foreach dir,$(SRC_DIRS),$(dir)/*.o) main.o *~ core

cleanall: clean
	rm -f $(PROJECT)
