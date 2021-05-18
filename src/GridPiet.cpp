#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>

#include "pietutil.hpp"
#include "pietboard.hpp"

#include "pietinterpreter.hpp"

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		std::cerr << "Usage : " << argv[0] << " [inputFileName] [outputFileName]" << std::endl;
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
	std::vector<std::string> lines;
	PietUtil::parseText(src.c_str(), cmds, lines);
	for (size_t i = 0; i < cmds.size(); i++)
	{
		std::cerr << lines[i] << " | " << cmds[i] << std::endl;
	}
	
	if(argc==2)
	{
		PietInterpreter pi;
		/*
		for (auto itr = cmds.begin(); itr != cmds.end(); itr++)
		{
			pi.appendCommand(*itr);
		}
		*/
		for (size_t i = 0; i < cmds.size(); i++)
		{
			pi.appendCommand(cmds[i], lines[i]);
		}
		
		std::cerr << pi.str_commands() << std::endl;
		
		while (pi.step())
		{
			std::cerr << pi.str() << std::endl;
		}
	}
	else if(argc>=3)
	{
		PietBoard pb;
		for (auto itr = cmds.begin(); itr != cmds.end(); itr++)
		{
			pb.appendCommand(*itr);
		}

		pb.put();
		std::cerr << pb.str() << std::endl;

		int W = 0;
		int H = 0;
		char* data = nullptr;
		pb.draw<char,1,'.'>(data,W,H,PietUtil::getPietColor_ascii);
		char* line = new char[W + 1];
		for (int y = 0; y < H; y++)
		{
			strncpy(line, data + y * W, W);
			line[W] = '\0';
			std::cerr << line << std::endl;
		}
		delete[] line;

		std::string ofname=argv[2];
		unsigned char* img = nullptr;
		pb.draw<unsigned char, 3, 255>(img, W, H, PietUtil::getPietColor);
		PietUtil::export_ppm(ofname.c_str(), img, W, H);
		delete[] img;
	}
	
	return 0;
}
