#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>

#include "pietutil.hpp"

#include "pietinterpreter.hpp"

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		std::cerr << "Usage : " << argv[0] << " [inputFileName]" << std::endl;
		return 0;
	}
	std::string ifname=argv[1];

	std::ifstream ifs(ifname);
	if (ifs.fail())
	{
		std::cerr << "!Error : File could not be opened." << std::endl;
		return 0;
	}
	std::string src((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	std::cerr << "=== Contents of Flow Description File (" << ifname << ") ===" << std::endl;
	std::cerr << src << std::endl;
	std::cerr << "=== /Contents of Flow Description File (" << ifname << ") ===" << std::endl;
	std::cerr << std::endl;

	std::vector<std::string> cmds;
	std::vector<std::string> lines;
	PietUtil::parseText(src.c_str(), cmds, lines);

	PietInterpreter pi;
	for (size_t i = 0; i < cmds.size(); i++)
	{
		pi.appendCommand(cmds[i], lines[i]);
	}

	std::cerr << pi.str_commands() << std::endl;
	std::cerr << std::endl;
	
	while (pi.step())
	{
		std::cerr << pi.str() << std::endl;
	}
	
	return 0;
}
