#include <array>
#include <cstdint>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/* Type utility */
using instr_args = std::vector<uint32_t*>;
using instr_func = std::function<void(instr_args)>;

/* Function declarations */
static void i_mov(const instr_args& registers);
static void i_add(const instr_args& registers);
static void i_sub(const instr_args& registers);
static void i_xor(const instr_args& registers);
static void i_cmp(const instr_args& registers);
static void i_je(const instr_args& registers);
static void i_jne(const instr_args& registers);

/* Registers */
static std::array<std::uint32_t, 4> reg = { 0 };
static std::uint32_t imm = 0;
static std::uint32_t zf = 0;;
static std::uint32_t pc = 0;

const static std::map<const std::string, instr_func> instructionMap
{
  {"MOV", i_mov},
  {"ADD", i_add},
  {"SUB", i_sub},
  {"XOR", i_xor},
  {"CMP", i_cmp},
  {"JE",  i_je},
  {"JNE", i_jne},
};

static std::map<std::string, uint32_t*> registerMap
{
  {"a",    &reg[0]},
  {"b",    &reg[1]},
  {"c",    &reg[2]},
  {"d",    &reg[3]},
  {"$imm", &imm}        // Unexistent register to represent immediates
};

/* Utility functions */
std::vector<std::string> split_string(const std::string& original)
{
	std::stringstream ss{ original };
	std::vector<std::string> v;

	std::string s; // temp string
	// Splitting stream with spaces
	while (std::getline(ss, s, ' ')) {
		v.push_back(s);
	}
	return v;
}

// https://stackoverflow.com/a/4654718
bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](unsigned char c) { return !std::isdigit(c) && c != '-'; }) == s.end();
}


/* Main */
int main()
{
	// Receive program
	std::vector<std::string> lines;
	/*
	std::string line;
	while (getline(std::cin, line)) {
		if (line.empty())
			break;
		lines.push_back(line);
	}
	*/
	std::ifstream file_in("input.txt");
	if (!file_in) {/*error*/ }

	std::string line;
	while (std::getline(file_in, line))
	{
		lines.push_back(line);
	}

	lines.erase(lines.begin());

	while (pc < lines.size())
	{
		// Extract instruction from string
		std::string call = *(lines.begin() + pc);
		std::vector<std::string> fullInstruction = split_string(call);
		std::string instruction = fullInstruction[0];

		// Find instruction in tree, check if valid
		auto instructionIt = instructionMap.find(instruction);
		if (instructionIt == instructionMap.end())
		{
			return -1;
		}

		instr_func function = instructionIt->second;

		// Check if the last "register" is an immediate; fill the immediate register and replace the number with the adress of $imm
		std::string lastInstr = fullInstruction.back();
		if (is_number(lastInstr))
		{
			imm = std::stoi(lastInstr);
			fullInstruction.back() = "$imm";
		}

		// Extract registers from string
		std::vector<std::uint32_t*> registers;
		for (auto it = fullInstruction.begin() + 1; it < fullInstruction.end(); it++)
		{
			auto regIt = registerMap.find(*it);
			if (regIt == registerMap.end())
			{
				return -1;
			}
			registers.push_back(regIt->second);
		}

		// Call function with arguments
		function(registers);
	}

	std::cout << reg[0] << '\n' << reg[1] << '\n' << reg[2] << '\n' << reg[3] << std::endl;
	return 0;
}


/* Functions to emulate */
static void i_mov(const instr_args& registers)
{
	if (registers.size() != 2)
		throw std::invalid_argument{ "MOV: Needs 2 arguments" };

	*registers[0] = *registers[1];
	pc++;
}

static void i_add(const instr_args& registers)
{
	if (registers.size() != 2)
		throw std::invalid_argument{ "ADD: Needs 2 arguments" };

	*registers[0] += *registers[1];
	pc++;
}

static void i_sub(const instr_args& registers)
{
	if (registers.size() != 2)
		throw std::invalid_argument{ "SUB: Needs 2 arguments" };

	*registers[0] -= *registers[1];
	pc++;
}

static void i_xor(const instr_args& registers)
{
	if (registers.size() != 2)
		throw std::invalid_argument{ "XOR: Needs 2 arguments" };

	*registers[0] ^= *registers[1];
	pc++;
}

static void i_cmp(const instr_args& registers)
{
	if (registers.size() != 2)
		throw std::invalid_argument{ "CMP: Needs 2 arguments" };

	zf = *registers[0] == *registers[1];
	pc++;
}

static void i_je(const instr_args& registers)
{
	if (registers.size() != 1)
		throw std::invalid_argument{ "JE: Needs 1 argument" };

	if (zf == 1)
		pc = *registers[0];
	else
		pc++;
}

static void i_jne(const instr_args& registers)
{
	if (registers.size() != 1)
		throw std::invalid_argument{ "JE: Needs 1 argument" };

	if (zf != 1)
		pc = *registers[0];
	else
		pc++;
}