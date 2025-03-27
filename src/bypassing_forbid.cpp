#include "simulator.hpp"
int main(int argc, char *argv[])
{
	
	bool forwarding_state=stoi(argv[1]);
	std::ifstream file(argv[2]);
	Simulator *riscv;
	if (file.is_open())
		riscv = new Simulator(file,forwarding_state);
	else
	{
		std::cerr << "File could not be opened.\n";
		return 0;
	}
	riscv->executeCommands();
	return 0;
}
