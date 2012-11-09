#include <iostream>
#include <fstream>
#include "txttools.h"

std::string removeQuotes(std::string in)
{
    return in.substr(1,in.length()-2);
}

void skipTo(std::ifstream &file, std::string value)
{
    std::string s;
    file >> s;
    while (!file.eof() && s != value)
    {
        file >> s;
    }
}