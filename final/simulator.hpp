#ifndef __SIMULATOR_HPP__
#define __SIMULATOR_HPP__
#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <boost/tokenizer.hpp>
#include "parser.hpp"
#include "decide_hazard.hpp"
#include "logic.hpp"

using namespace std;

struct Simulator
{	

	struct Parser* parser;
	struct Pipeline* pipeline;
	int registers[32] = {0}, PCcurr = 0, PCnext;
	std::unordered_map<std::string, std::function<int(Simulator &, std::string, std::string, std::string)>> instructions;
	std::unordered_map<std::string, int> registerMap, address;
	static const int MAX = (1 << 20);
	int data[MAX >> 2] = {0};
	std::unordered_map<int, int> memoryDelta;
	std::vector<std::vector<std::string>> commands;
	std::vector<int> commandCount;
	struct UpdateQueue* memoryupdatequeue;
	enum exit_code
	{
		SUCCESS = 0,
		INVALID_REGISTER,
		INVALID_LABEL,
		INVALID_ADDRESS,
		SYNTAX_ERROR,
		MEMORY_ERROR
	};


	// constructor to initialise the instruction set
	Simulator(std::ifstream &file,bool forwarding_state)
	{
		instructions = {{"add", &Simulator::add}, {"sub", &Simulator::sub},{"and", &Simulator::And}, {"or", &Simulator::Or},
                 {"mul", &Simulator::mul},{"div",&Simulator::div}, {"beq", &Simulator::beq},
                {"bne", &Simulator::bne},{"bge", &Simulator::bge},{"blt",&Simulator::blt}, {"slt", &Simulator::slt}, {"jal", &Simulator::jal},
                {"lw", &Simulator::lw}, {"sw", &Simulator::sw},{"addi", &Simulator::addi},
                {"slli", &Simulator::slli}, {"srli", &Simulator::srli},{"srai",&Simulator::srai}, {"jalr", &Simulator::jalr},{"lb",  &Simulator::lb},
				{"sb",  &Simulator::sb}};
		parser = new Parser(file,forwarding_state);
	    commands=parser->commands;
		registerMap = parser->registerMap;
		address = parser->address;
		commandCount=parser->commandCount;
		memoryupdatequeue=new UpdateQueue();
		pipeline=parser->pipeline;
	}

	
	// perform add operation
	int add(std::string r1, std::string r2, std::string r3)
	{
        if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0){
			return 1;
		}
		registers[registerMap[r1]] = registers[registerMap[r2]]+registers[registerMap[r3]];
		PCnext = PCcurr + 1;
		return 0;
	}
    // perform subtraction operation
    int sub(std::string r1, std::string r2, std::string r3)
    {
        if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0){
            return 1;
        }
        registers[registerMap[r1]] = registers[registerMap[r2]]-registers[registerMap[r3]];
        PCnext = PCcurr + 1;
        return 0;
    }
    // perform and operation
	int And(std::string r1, std::string r2, std::string r3)
	{
        if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0){
			return 1;
		}
		registers[registerMap[r1]] = (registers[registerMap[r2]])&(registers[registerMap[r3]]);
		PCnext = PCcurr + 1;
		return 0;
	}// perform or operation
	int Or(std::string r1, std::string r2, std::string r3)
	{
        if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0){
			return 1;
		}
		registers[registerMap[r1]] = (registers[registerMap[r2]])||(registers[registerMap[r3]]);
		PCnext = PCcurr + 1;
		return 0;
	}
	// perform multiplication operation
	int mul(std::string r1, std::string r2, std::string r3)
	{
        if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0){
			return 1;
		}
		registers[registerMap[r1]] = registers[registerMap[r2]]*registers[registerMap[r3]];
		PCnext = PCcurr + 1;
		return 0;
	}
    int div(std::string r1, std::string r2, std::string r3)
    {
        if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0){
            return 1;
        }
        registers[registerMap[r1]] = registers[registerMap[r2]]/registers[registerMap[r3]];
        PCnext = PCcurr + 1;
        return 0;
    }

    // perform add immediate operation
	int addi(std::string r1, std::string r2, std::string num)
	{
		if (!checkRegisters({r1, r2}) || registerMap[r1] == 0){
			return 1;
		}
		try
		{
			registers[registerMap[r1]] = registers[registerMap[r2]] + stoi(num);
			PCnext = PCcurr + 1;
			return 0;
		}
		catch (std::exception &e)
		{
			return 4;
		}
	}
    int andi(std::string r1, std::string r2, std::string num)
    {
        if (!checkRegisters({r1, r2}) || registerMap[r1] == 0){
            return 1;
        }
        try
        {
            registers[registerMap[r1]] = registers[registerMap[r2]] & stoi(num);
            PCnext = PCcurr + 1;
            return 0;
        }
        catch (std::exception &e)
        {
            return 4;
        }
    }
    int ori(std::string r1, std::string r2, std::string num)
    {
        if (!checkRegisters({r1, r2}) || registerMap[r1] == 0){
            return 1;
        }
        try
        {
            registers[registerMap[r1]] = registers[registerMap[r2]] || stoi(num);
            PCnext = PCcurr + 1;
            return 0;
        }
        catch (std::exception &e)
        {
            return 4;
        }
    }
	// perform shift left operation
	int slli(std::string r1, std::string r2, std::string num)
	{
		if (!checkRegisters({r1, r2}) || registerMap[r1] == 0){
			return 1;
		}
		try
		{
			registers[registerMap[r1]] = (registers[registerMap[r2]] << stoi(num));
			PCnext = PCcurr + 1;
			return 0;
		}
		catch (std::exception &e)
		{
			return 4;
		}
	}
	// perform shift right operation
	int srli(std::string r1, std::string r2, std::string num)
	{
		if (!checkRegisters({r1, r2}) || registerMap[r1] == 0){
			return 1;
		}
		try
		{
			registers[registerMap[r1]] = (registers[registerMap[r2]] >> stoi(num));
			PCnext = PCcurr + 1;
			return 0;
		}
		catch (std::exception &e)
		{
			return 4;
		}
	}	
    int srai(std::string r1, std::string r2, std::string num)
    {
        if (!checkRegisters({r1, r2}) || registerMap[r1] == 0){
            return 1;
        }
        try
        {
            registers[registerMap[r1]] = (registers[registerMap[r2]] >> stoi(num));
            PCnext = PCcurr + 1;
            return 0;
        }
        catch (std::exception &e)
        {
            return 4;
        }
    }

	// perform the beq operation
	int beq(std::string r1, std::string r2, std::string label)
	{
		return perform_op(r1, r2, label, [](int a, int b)
				   { return a == b; });
	}
	// perform the bne operation
	int bne(std::string r1, std::string r2, std::string label)
	{
		return perform_op(r1, r2, label, [](int a, int b)
				   { return a != b; });
	}
    int bge(std::string r1, std::string r2, std::string label){
        return perform_op(r1, r2, label, [](int a, int b)
                   { return a >= b; });
    }
    int blt(std::string r1, std::string r2, std::string label){
        return perform_op(r1, r2, label, [](int a, int b)
                   { return a < b; });
    }
	// implements beq and bne by taking the comparator
	int perform_op(std::string r1, std::string r2, std::string branch_operand, std::function<bool(int, int)> comp)
	{
		if (!checkRegisters({r1, r2}))
			return 1;
		try {
			// Try interpreting branch_operand as an immediate offset.
			int offset = (stoi(branch_operand)/4);
            if(offset==0){
                offset=1;
            }
			PCnext = comp(registers[registerMap[r1]], registers[registerMap[r2]]) ? PCcurr + offset : PCcurr + 1;
            
			return 0;
		}
		catch (std::exception &e) {
			// Fallback: interpret branch_operand as a label.
			if (!checkLabel(branch_operand))
				return 4; // Syntax error.
			if (address.find(branch_operand) == address.end() || address[branch_operand] == -1)
				return 2; // Label not found or defined too many times.
			PCnext = comp(registers[registerMap[r1]], registers[registerMap[r2]]) ? address[branch_operand] : PCcurr + 1;
			return 0;
		}
	}

	// implements slt operation
	int slt(std::string r1, std::string r2, std::string r3)
	{
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0){
			return 1;
		}
		registers[registerMap[r1]] = registers[registerMap[r2]] < registers[registerMap[r3]];
		PCnext = PCcurr + 1;
		return 0;
	}

	// Implements both j (jump) and jal (jump and link)
	int jal(std::string rd, std::string offset_str, std::string unused2)
	{
		try {
			// Convert the offset from string to integer.
			int offset = stoi(offset_str);
			
			// Validate that the destination register is defined.
			if (registerMap.find(rd) == registerMap.end())
				return 1; // error: invalid register

			// For jal (jump and link), if rd is not x0, set rd to return address (PCcurr + 1).
			if (registerMap[rd] != 0) {
				registers[registerMap[rd]] = PCcurr + 1;
			}
			
			// Update the next program counter.
			PCnext = PCcurr + (offset/4);
            if(PCnext==PCcurr){
                PCnext=PCcurr+1;
            }
			return 0;
		}
		catch (std::exception &e) {	
			return 4;  // error in immediate conversion
		}
	}

	int jalr(std::string rd, std::string rs1, std::string offset_str)
	{
		// Check that both registers are valid.
		if (!checkRegisters({rd, rs1}))
			return 1;
		try {
			int offset = stoi(offset_str);
			// Calculate the target address as the sum of the content of rs1 and the immediate offset.
			int target = registers[registerMap[rs1]] + (offset/4);
			
			// Update the link if needed: if rd is not x0, store PCcurr+1 in rd.
			if (registerMap[rd] != 0)
				registers[registerMap[rd]] = PCcurr + 1;
            
            PCnext = target;
            
			return 0;
		}
		catch (std::exception &e) {
			return 4;
		}
	}

	// perform load word operation
	int lw(std::string r, std::string in1, std::string in2)
	{
		std::string location;
		if(std::find(in1.begin(),in1.end(),'x')!=in1.end()){
			location=in1;
		}
		else{
			location=in1+"("+in2+")";
		}
		if (!checkRegister(r) || registerMap[r] == 0){
			return 1;
		}
		int address = locateAddress(location);
		if (address < 0){
			return abs(address);
		}
		registers[registerMap[r]] = data[address];
		PCnext = PCcurr + 1;
		return 0;
	}
	// perform store word operation
	int sw(std::string r, std::string in1, std::string in2)
	{
		std::string location;
		if(std::find(in1.begin(),in1.end(),'x')!=in1.end()){
			location=in1;
		}
		else{
			location=in1+"("+in2+")";
		}
		if (!checkRegister(r)){
			return 1;
		}
		int address = locateAddress(location);
		if (address < 0){
			return abs(address);
		}
		if (data[address] != registers[registerMap[r]]){
			memoryDelta[address] = registers[registerMap[r]];
		}
		data[address] = registers[registerMap[r]];
		PCnext = PCcurr + 1;
		return 0;
	}

	// Load Byte: Loads a single byte (with sign-extension) into register r.
    int lb(std::string r, std::string in1, std::string in2)
    {
        std::string location;
        if(std::find(in1.begin(), in1.end(), 'x') != in1.end()){
            location = in1;
        }
        else{
            location = in1 + "(" + in2 + ")";
        }
        if (!checkRegister(r) || registerMap[r] == 0)
            return 1;
        int addr = 0;
        try {
            size_t lparen = location.find('(');
            if(lparen != std::string::npos){
                int offset = (lparen > 0) ? stoi(location.substr(0, lparen)) : 0;
                std::string reg = location.substr(lparen+1);
                reg.pop_back(); // remove trailing ')'
                if(!checkRegister(reg))
                    return 1;
                addr = registers[registerMap[reg]] + offset;
            } else {
                addr = stoi(location);
            }
        } catch(std::exception &e) {
            return 4;
        }
        // For lb, no alignment check is needed.
        int word_index = addr / 4;
        int byte_offset = addr % 4;
        if(word_index < 0 || word_index >= (MAX >> 2)) return -3;
        int word = data[word_index];
        int byte_val = (word >> (8 * byte_offset)) & 0xff;
        // Sign-extend if the most significant bit is set.
        if (byte_val & 0x80) byte_val |= ~0xff;
        registers[registerMap[r]] = byte_val;
        PCnext = PCcurr + 1;
        return 0;
    }

    // Store Byte: Stores the least-significant byte from register r into memory.
    int sb(std::string r, std::string in1, std::string in2)
    {
        std::string location;
        if(std::find(in1.begin(), in1.end(), 'x') != in1.end()){
            location = in1;
        }
        else{
            location = in1 + "(" + in2 + ")";
        }
        if (!checkRegister(r))
            return 1;
        int addr = 0;
        try {
            size_t lparen = location.find('(');
            if(lparen != std::string::npos){
                int offset = (lparen > 0) ? stoi(location.substr(0, lparen)) : 0;
                std::string reg = location.substr(lparen+1);
                reg.pop_back();
                if(!checkRegister(reg))
                    return 1;
                addr = registers[registerMap[reg]] + offset;
            } else {
                addr = stoi(location);
            }
        } catch(std::exception &e) {
            return 4;
        }
        int word_index = addr / 4;
        int byte_offset = addr % 4;
        if(word_index < 0 || word_index >= (MAX >> 2)) return -3;
        int old_word = data[word_index];
        int mask = ~(0xff << (8 * byte_offset));
        int cleared_word = old_word & mask;
        int byte_val = registers[registerMap[r]] & 0xff;
        int new_word = cleared_word | (byte_val << (8 * byte_offset));
        data[word_index] = new_word;
        // Optionally update memoryDelta if you want to track memory changes:
        memoryDelta[word_index] = new_word;
        PCnext = PCcurr + 1;
        return 0;
    }

    // Load Halfword: Loads 16 bits from memory (with sign-extension) into register r.
    int lh(std::string r, std::string in1, std::string in2)
    {
        std::string location;
        if (in1.find('x') != std::string::npos) {
            location = in1;
        } else {
            location = in1 + "(" + in2 + ")";
        }
        if (!checkRegister(r) || registerMap[r] == 0)
            return 1;
        int addr = 0;
        try {
            size_t lparen = location.find('(');
            if (lparen != std::string::npos) {
                int offset = (lparen > 0) ? stoi(location.substr(0, lparen)) : 0;
                std::string reg = location.substr(lparen + 1);
                reg.pop_back(); // remove trailing ')'
                if (!checkRegister(reg))
                    return 1;
                addr = registers[registerMap[reg]] + offset;
            } else {
                addr = stoi(location);
            }
        }
        catch (std::exception &e) {
            return 4;
        }
        
        // Check that the address is halfword-aligned (multiple of 2)
        if (addr % 2 != 0 || addr < 0 || addr >= MAX)
            return -3;
        
        // Convert word-based memory into a halfword access.
        int word_index = addr / 4;
        // Determine whether we are reading the lower (0) or upper (1) halfword.
        int halfword_offset = (addr % 4) / 2;
        int word = data[word_index];
        int halfword;
        if (halfword_offset == 0) {
            halfword = word & 0xFFFF;  // lower 16 bits
        } else {
            halfword = (word >> 16) & 0xFFFF;  // upper 16 bits
        }
        
        // Sign-extend the 16-bit halfword to 32 bits.
        if (halfword & 0x8000)
            halfword |= ~0xFFFF;
        
        registers[registerMap[r]] = halfword;
        PCnext = PCcurr + 1;
        return 0;
    }

    // Store Halfword: Stores the least-significant 16 bits from register r into memory.
    int sh(std::string r, std::string in1, std::string in2)
    {
        std::string location;
        if (in1.find('x') != std::string::npos) {
            location = in1;
        } else {
            location = in1 + "(" + in2 + ")";
        }
        if (!checkRegister(r))
            return 1;
        int addr = 0;
        try {
            size_t lparen = location.find('(');
            if (lparen != std::string::npos) {
                int offset = (lparen > 0) ? stoi(location.substr(0, lparen)) : 0;
                std::string reg = location.substr(lparen + 1);
                reg.pop_back(); // remove trailing ')'
                if (!checkRegister(reg))
                    return 1;
                addr = registers[registerMap[reg]] + offset;
            } else {
                addr = stoi(location);
            }
        }
        catch (std::exception &e) {
            return 4;
        }
        
        // Check that the address is halfword-aligned.
        if (addr % 2 != 0 || addr < 0 || addr >= MAX)
            return -3;
        
        int word_index = addr / 4;
        int halfword_offset = (addr % 4) / 2;
        int old_word = data[word_index];
        int value_to_store = registers[registerMap[r]] & 0xFFFF;
        int new_word;
        if (halfword_offset == 0) {
            // Clear lower 16 bits and insert the halfword.
            int mask = 0xFFFF0000;
            new_word = (old_word & mask) | value_to_store;
        } else {
            // Clear upper 16 bits and insert the halfword.
            int mask = 0x0000FFFF;
            new_word = (old_word & mask) | (value_to_store << 16);
        }
        data[word_index] = new_word;
        memoryDelta[word_index] = new_word; // if tracking memory changes
        PCnext = PCcurr + 1;
        return 0;
    }




	
	int locateAddress(std::string location)
	{
		if (location.back() == ')')
		{
			try
			{
				int lparen = location.find('(');
				int offset = stoi(lparen == 0 ? "0" : location.substr(0, lparen));
				std::string reg = location.substr(lparen + 1);
				reg.pop_back(); // remove trailing ')'
				if (!checkRegister(reg)){
					return -3;
				}
				int address = registers[registerMap[reg]] + offset;
				// Updated check: only require alignment and that address is in valid range [0, MAX)
				if (address % 4 || address < 0 || address >= MAX){
					return -3;
				}
				return address / 4;
			}
			catch (std::exception &e)
			{
				return -4;
			}
		}
		try
		{
			int address = stoi(location);
			if (address % 4 || address < 0 || address >= MAX){
				return -3;
			}
			return address / 4;
		}
		catch (std::exception &e)
		{
			return -4;
		}
	}
	
	
	// checks if label is valid
	inline bool checkLabel(std::string str)
	{
		return str.size() > 0 && isalpha(str[0]) && all_of(++str.begin(), str.end(), [](char c)
														   { return (bool)isalnum(c); }) &&
			   instructions.find(str) == instructions.end();
	}
	// checks if the register is a valid one
	inline bool checkRegister(std::string r)
	{
		return registerMap.find(r) != registerMap.end();
	}
	// checks if all of the registers are valid or not
	bool checkRegisters(std::vector<std::string> regs)
	{
		return std::all_of(regs.begin(), regs.end(), [&](std::string r)
						   { return checkRegister(r); });
	}
	/*
		handle all exit codes:
		0: correct execution
		1: register provided is incorrect
		2: invalid label
		3: unaligned or invalid address
		4: syntax error
		5: commands exceed memory limit
	*/
	void handleExit(exit_code code, int cycleCount)
	{
		std::cout << '\n';
		switch (code)
		{
		case 1:
			std::cerr << "Invalid register provided or syntax error in providing register\n";
			break;
		case 2:
			std::cerr << "Label used not defined or defined too many times\n";
			break;
		case 3:
			std::cerr << "Unaligned or invalid memory address specified\n";
			break;
		case 4:
			std::cerr << "Syntax error encountered\n";
			break;
		case 5:
			std::cerr << "Memory limit exceeded\n";
			break;
		default:
			break;
		}
		if (code != 0)
		{
			std::cerr << "Error encountered at:\n";
			for (auto &s : commands[PCcurr])
				std::cerr << s << ' ';
			std::cerr << '\n';
		}
		// print_final_output();
	}

	// execute the commands sequentially (no pipelining)
	void executeCommands()
	{
		if (commands.size() >= MAX / 4)
		{
			handleExit(MEMORY_ERROR, 0);
			return;
		}

		int clockCyclesUnpipelined = 0;
        int count=0;
		while (PCcurr < commands.size())
		{
			++clockCyclesUnpipelined;
			std::vector<std::string> &command = commands[PCcurr];
			if (instructions.find(command[0]) == instructions.end())
			{
				handleExit(SYNTAX_ERROR, clockCyclesUnpipelined);
				return;
			}
			exit_code ret = (exit_code) instructions[command[0]](*this, command[1], command[2], command[3]);
			if (ret != SUCCESS)
			{
				handleExit(ret, clockCyclesUnpipelined);
				return;
			}
			++commandCount[PCcurr];
			parser->parametric_commands[PCcurr]->value=registers[registerMap[command[1]]];
			pipeline->run_command(parser->parametric_commands[PCcurr]);
			pipeline->save();
			if(command[0]=="beq" || command[0]=="bne" || command[0]=="bge" || command[0]=="blt" || command[0]=="jal" || command[0]=="jalr"){
				pipeline->insert_halt(parser->parametric_commands[PCcurr]);
			}
			if(command[0]=="sw" || command[0]=="sb" || command[0]=="lw" || command[0]=="lb"){
				updatememory(command,pipeline->history[(int)pipeline->history.size()-1]->stages[parser->parametric_commands[PCcurr]->readindex][1]);
			}
			PCcurr = PCnext;
            count++;
            if(count>20){
                break;
            }
		}
		// handleExit(SUCCESS, clockCyclesUnpipelined);
		// pipeline->print_table();
        std::ofstream outfile("output.txt");
        if (!outfile.is_open()) {
            std::cerr << "Error opening output.txt for writing." << std::endl;
        } else {
            pipeline->print_table(outfile);
            outfile.close();
        }
	}

	void updatememory(std::vector<std::string> command,int time){
		std::string location;
		if(std::find(command[2].begin(),command[2].end(),'x')!=command[2].end()){
			location=command[2];
		}
		else{
			location=command[2]+"("+command[3]+")";
		}
		int address = locateAddress(location);
		memoryupdatequeue->enqueue(address,registers[registerMap[command[1]]],time);
	}

	

};

#endif