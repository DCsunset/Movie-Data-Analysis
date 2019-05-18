CXX = g++
CXXFLAGS = -g -std=c++17 -pthread -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wlogical-op -Wuseless-cast -Wformat=2

HEADERS = $(shell find . -name "*.hpp") #To find headers to any depth
SOURCES = $(shell find . -name "*.cpp")
ALLOBJS = $(shell find . -name "*.o")
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all
all : main

%.o : %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

main : main.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

.PHONY: clean
clean :
	rm -f $(ALLOBJS) *~ .*~ main *.json

