#include <iostream>

#include "arg.hpp"

int main(int argc, char** argv)
{
    Arg arg;
    arg.appendOption("-i", "inputFileName");
    arg.appendOption("--inputFileName", "inputFileName");
    arg.appendOption("-o", "outputFileName");
    arg.appendOption("--outputFileName", "outputFileName");
    
    std::cerr << arg.help() << std::endl;
    
    return 0;
}
