
PROJECT = game
TEST_PROJECT = test-game

CXX = g++
CXXFLAGS = -I. -ICards -ICore -ICore/events -IData -IPlayers -ISystems -IUI -std=c++17
LIBS = -lm
TESTFLAGS = -lgtest -lgmock -pthread

SRC_DIRS = Cards Core Core/events Data Players Systems UI

SRCS = main.cpp $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(SRCS:.cpp=.o)

TEST_SRC = tests/test.cpp
TEST_SRCS = $(filter-out main.cpp, $(SRCS))
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

.PHONY: all clean cleanall test

all: $(PROJECT)

$(PROJECT): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_PROJECT)

$(TEST_PROJECT): $(TEST_SRC) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(TESTFLAGS)

clean:
	rm -f $(OBJS) $(TEST_OBJS) *~ core $(TEST_PROJECT)

cleanall: clean
	rm -f $(PROJECT)
