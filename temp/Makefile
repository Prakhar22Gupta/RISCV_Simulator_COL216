compile : mc_translator.hpp logic.hpp parser.hpp simulator.hpp decide_hazard.hpp register_file.hpp bypassing_forbid.cpp bypassing.cpp 
	g++ bypassing_forbid.cpp -o bypassing_forbid.o
	g++ bypassing.cpp -o bypassing.o
	g++ main.cpp -o main.o
	./main.o

clean:
	rm bypassing_forbid.o
	rm bypassing.o
	rm main.o

bypassing_forbid: register_file.hpp logic.hpp parser.hpp simulator.hpp decide_hazard.hpp bypassing_forbid.cpp
	g++ bypassing_forbid.cpp -o bypassing_forbid.o

bypassing: register_file.hpp logic.hpp parser.hpp simulator.hpp decide_hazard.hpp bypassing.cpp
	g++ bypassing.cpp -o bypassing.o

bypass_forbid: bypassing_forbid
	./bypassing_forbid.o 0 input.txt 30

bypass: bypassing
	./bypassing.o 1 input.txt 30

