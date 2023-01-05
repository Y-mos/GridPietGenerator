#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>

#include "pietutil.hpp"
#include "pietboard.hpp"

#include "arg.hpp"

void outputPietDot(std::ostream& ost, const PietBoard& pb)
{
    ost << pb.getDot() << std::endl;
}

void outputPietCSV(std::ostream& ost,const PietBoard& pb)
{
    ost << pb.getList(',') << std::endl;
    return;
}


void outputSrcRaw(std::ostream& ost,const std::string& src,const std::string& ifname)
{
    ost << "=== Contents of Flow Description File (" << ifname << ") ===" << std::endl;
    ost << src << std::endl;
    ost << "=== /Contents of Flow Description File (" << ifname << ") ===" << std::endl;
    ost << std::endl;
}

void outputCommandList(std::ostream& ost,const std::vector<std::string>& cmds, const std::vector<std::string>& lines)
{
    ost << "=== Commands ===" << std::endl;
    for (size_t i = 0; i < cmds.size(); i++)
    {
        ost << lines[i] << " | " << cmds[i] << std::endl;
    }
    ost << "=== /Commands ===" << std::endl;
    ost << std::endl;
}
void outputPietBoard(std::ostream& ost,const PietBoard& pb)
{
    ost << pb.str() << std::endl;
}

void outputPietAsciiDescription(std::ostream& ost, const PietBoard& pb, const char* (*getPietColor)(int h, int b, const void* obj), const char* (*getPathColor)(int dir, const void* obj),bool isHeader=false)
{
    if(isHeader)
    {
        ost << "=== Output layout in ASCII ===" << std::endl;
    }
    
    int W = 0;
    int H = 0;
    char* data = nullptr;
    pb.draw<char, 1, '.'>(data, W, H, getPietColor, getPathColor);
    char* line = new char[W + 1];
    ost << "PIET TEXT DESCRIPTION" << std::endl;
    ost << W << "," << H << std::endl;
    for (int y = 0; y < H; y++)
    {
        strncpy(line, data + y * W, W);
        line[W] = '\0';
        ost << line << std::endl;
    }
    delete[] line;
    delete[] data;

    if(isHeader)
    {
        ost << "=== /Output layout in ASCII ===" << std::endl;
        ost << std::endl;
    }
}

void outputPietPmm(std::string ofname,const PietBoard& pb)
{
    int W = 0;
    int H = 0;
    unsigned char* img = nullptr;
    pb.draw<unsigned char, 3, 255>(img, W, H, PietUtil::getPietColor);
    PietUtil::export_ppm(ofname.c_str(), img, W, H);
    delete[] img;
}

int main(int argc, char** argv)
{
    Arg arg;
    arg.registerKey("outputPietFile","output path for piet source code", true);
    arg.registerKey("outputDesignFile", "output path for piet ascii file", true);
    arg.registerKey("outputDebugFile","output path for debug log",true);
    arg.registerKey("outputCSV","output path for piet design information in CSV format",true);
    arg.registerKey("outputDot","output path for dot file",true);

    arg.addOption("-o", "outputPietFile");
    arg.addOption("--output","outputPietFile");
    arg.addOption("-a", "outputDesignFile");
    arg.addOption("--ascii","outputDesignFile");
    arg.addOption("-l", "outputDebugFile");
    arg.addOption("--log", "outputDebugFile");
    arg.addOption("-c","outputCSV");
    arg.addOption("--csv","outputCSV");
    arg.addOption("-d","outputDot");
    arg.addOption("--dot","outputDot");

	if(argc < 2)
	{
		std::cerr << "Usage : " << argv[0] << " [inputFileName] ..." << std::endl;
        std::cerr << arg.help() << std::endl;
		return 0;
	}
    arg.parse(argc-1, argv+1);
    
    std::string ifname=argv[1];
    std::string ofname=((arg.get("outputPietFile")=="")?(ifname+".ppm"):(arg.get("outputPietFile")));

	std::ifstream ifs(ifname);
	if (ifs.fail())
	{
		std::cerr << "!Error : File could not be opened." << std::endl;
		return 0;
	}
    
	std::string src((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    outputSrcRaw(std::cerr, src, ifname);

	std::vector<std::string> cmds;
	std::vector<std::string> lines;
	PietUtil::parseText(src.c_str(), cmds, lines);
    outputCommandList(std::cerr,cmds,lines);
    
	PietBoard pb;
    int cmdNo=0;
	for (auto itr = cmds.begin(); itr != cmds.end(); itr++)
	{
		pb.appendCommand(*itr,cmdNo);
        cmdNo++;
	}
    
	pb.put();
    outputPietBoard(std::cerr,pb);
    if(arg.get("outputDebugFile")!="")
    {
        std::ofstream ofs(arg.get("outputDebugFile"));
        if (ofs.is_open())
        {
            outputPietBoard(ofs,pb);
        }

    }

    outputPietAsciiDescription(std::cerr, pb, PietBlock::getPietBlockHash, PietPath::getPathColor_ascii, true);

	if (arg.get("outputDesignFile")!="")
	{
        std::ofstream ofs(arg.get("outputDesignFile"));
        if (ofs.is_open())
        {
            outputPietAsciiDescription(ofs, pb, PietUtil::getPietColor_ascii, PietPath::getPathColor_ascii);
        }
	}
    
    if(arg.get("outputCSV")!="")
    {
        std::ofstream ofs(arg.get("outputCSV"));
        if (ofs.is_open())
        {
            outputPietCSV(ofs,pb);
        }
    }
    
    if(arg.get("outputDot")!="")
    {
        std::ofstream ofs(arg.get("outputDot"));
        if (ofs.is_open())
        {
            outputPietDot(ofs,pb);
        }
    }

    
    outputPietPmm(ofname,pb);

	return 0;
}
