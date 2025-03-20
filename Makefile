all: pipelinesimulator main.cpp pipeline.hpp parser.hpp simulator.hpp configuration.hpp requirements.hpp 5stage.cpp 5stage_bypass.cpp 

compile : main.cpp pipeline.hpp parser.hpp simulator.hpp configuration.hpp requirements.hpp 5stage.cpp 5stage_bypass.cpp 
	g++ main.cpp -o pipelinesimulator.o
	g++ 5stage.cpp -o pipelinesimulator5.o
	g++ 5stage_bypass.cpp -o pipelinesimulator5b.o

clean:
	rm pipelinesimulator
	rm pipelinesimulator5
	rm pipelinesimulator5b

pipelinesimulator: requirements.hpp pipeline.hpp parser.hpp simulator.hpp configuration.hpp main.cpp
	g++ main.cpp -o pipelinesimulator.o

pipelinesimulator5: requirements.hpp pipeline.hpp parser.hpp simulator.hpp configuration.hpp 5stage.cpp
	g++ 5stage.cpp -o pipelinesimulator5.o

pipelinesimulator5b: requirements.hpp pipeline.hpp parser.hpp simulator.hpp configuration.hpp 5stage_bypass.cpp
	g++ 5stage_bypass.cpp -o pipelinesimulator5b.o

run_5stage: pipelinesimulator5
	./pipelinesimulator5.o 1 input.asm

run_5stage_bypass: pipelinesimulator5b
	./pipelinesimulator5b.o 2 input.asm

main: pipelinesimulator
	./pipelinesimulator 1 input.asm
	./pipelinesimulator 2 input.asm
	./pipelinesimulator 3 input.asm
	./pipelinesimulator 4 input.asm
