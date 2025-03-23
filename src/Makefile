compile : logic.hpp parser.hpp simulator.hpp decide_hazard.hpp register_file.hpp no_bypass.cpp bypassing.cpp 
	g++ no_bypass.cpp -o pipelinesimulator5.o
	g++ bypassing.cpp -o pipelinesimulator5b.o

clean:
	rm pipelinesimulator5.o
	rm pipelinesimulator5b.o

pipelinesimulator5: register_file.hpp logic.hpp parser.hpp simulator.hpp decide_hazard.hpp no_bypass.cpp
	g++ no_bypass.cpp -o pipelinesimulator5.o

pipelinesimulator5b: register_file.hpp logic.hpp parser.hpp simulator.hpp decide_hazard.hpp bypassing.cpp
	g++ bypassing.cpp -o pipelinesimulator5b.o

run_no_bypass: pipelinesimulator5
	./pipelinesimulator5.o input.txt

run_bypass: pipelinesimulator5b
	./pipelinesimulator5b.o input.txt

