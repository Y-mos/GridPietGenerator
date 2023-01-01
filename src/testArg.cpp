#include <iostream>

#include "arg.hpp"

int main(int argc, char** argv)
{
    Arg arg;
    arg.registerKey("inputFileName", "input file path", true);
    arg.registerKey("outputFileName", "output file path", true);

    arg.addOption("-i", "inputFileName");
    arg.addOption("-o", "outputFileName");
    arg.addOption("--inputFile", "inputFileName");
    arg.addOption("--outputFile", "outputFileName");

    std::cerr << arg.help() << std::endl;
    arg.parse(argc, argv);
    std::cerr << arg.resultList() << std::endl;

    return 0;
}
