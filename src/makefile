CXX = g++
CXXFLAGS = -std=c++17 -Wall

all: noforward forward

noforward: main.cpp processor.cpp pipeline.cpp alu.cpp
	$(CXX) $(CXXFLAGS) -o noforward main.cpp processor.cpp pipeline.cpp alu.cpp

forward: main.cpp processor.cpp pipeline.cpp alu.cpp
	$(CXX) $(CXXFLAGS) -DFORWARDING -o forward main.cpp processor.cpp pipeline.cpp alu.cpp

clean:
	rm -f noforward forward
