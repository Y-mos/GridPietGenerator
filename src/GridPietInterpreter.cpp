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
	if(argc < 1)
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
	std::cerr << src << std::endl;

	std::vector<std::string> cmds;
	PietUtil::parseText(src.c_str(), cmds);
	for (std::string c : cmds) { std::cerr << c << std::endl; }
	
	PietInterpreter pi;
	for (auto itr = cmds.begin(); itr != cmds.end(); itr++)
	{
		pi.appendCommand(*itr);
	}
	
	std::cerr << pi.str_commands() << std::endl;
	
	while (pi.step())
	{
		std::cerr << pi.str() << std::endl;
	}
	
	return 0;
}
