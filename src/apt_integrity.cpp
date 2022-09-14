#include "apt_integrity.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace apt_integrity;

std::vector<std::string> apt_int::splitString(const std::string& str)
{
    std::stringstream ss(str);
    std::vector<std::string> tokens;
    std::string temp;
    while(ss>>temp)
    {
        tokens.push_back(temp);
    }
    return tokens;
}

void apt_int::readSourceFile() {
    std::vector<source> sources;
    // std::ifstream sourceList("/etc/apt/mirror.list");
    std::ifstream sourceList("./mirror.list");
    std::string line;
    while(sourceList)
    {
        std::getline(sourceList,line);
        try
        {
            if (line.starts_with("deb"))
            {
                source currentSource;
                std::vector<std::string> tokens = splitString(line);
                int currToken = 2;
                if(tokens[1].starts_with("[arch="))
                {
                    currentSource.arch = tokens[1].substr(6,tokens[1].find(']')-6);
                    currentSource.source = tokens[2];
                    currToken++;
                }
                else
                {
                    currentSource.source = tokens[1];
                }
                currentSource.distro = tokens[currToken];
                currToken++;
                while (currToken<tokens.size())
                {
                    currentSource.scopes.push_back(tokens[currToken]);
                    currToken++;
                }
                this->_sources.push_back(currentSource);
            }
        }
        catch (...)
        {
            std::cerr << "Error Parsing line in source list:" << line << "\n";
            exit(1);
        }
    }
}

int apt_int::main()
{
    readSourceFile();
}