#include "simulator.hpp"
int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Required argument: question number and file_name\n./RISCV_interpreter <file name>\n";
		return 0;
	}
	std::ifstream file(argv[1]);
	Simulator *riscv;
	if (file.is_open())
		riscv = new Simulator(file);
	else
	{
		std::cerr << "File could not be opened. Terminating...\n";
		return 0;
	}
	riscv->executeCommands();
	return 0;
}
