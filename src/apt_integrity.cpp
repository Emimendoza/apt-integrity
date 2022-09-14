#include "apt_integrity.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <sys/file.h>
#include <unistd.h>
#include <openssl/sha.h>

using namespace apt_integrity;

// Helper functions
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
std::string apt_int::sha256(const std::string &path) {
    return std::string();
}
// Main Functions
void apt_int::readSourceFile() {
    std::vector<source> sources;
    // std::ifstream sourceList("/etc/apt/mirror.list");
    std::ifstream sourceList("./mirror.list");
    std::string line;
    while(sourceList)
    {
        std::getline(sourceList,line);
        std::vector<std::string> tokens = splitString(line);
        try
        {
            if (line.starts_with("deb"))
            {
                source currentSource;
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
            if (line.starts_with("set"))
            {
                if (tokens[1] == "base_path")
                    _basePath = std::string(tokens[2]);
                if (tokens[1] == "mirror_path")
                    _mirrorPath = std::string(tokens[2]);
                if (tokens[1] == "var_path")
                    _varPath = std::string(tokens[2]);
                if (tokens[1] == "nthreads")
                    _threads = std::stoi(tokens[2]);
            }
        }
        catch (...)
        {
            std::cerr << "Error Parsing line in source list:" << line << "\n";
            exit(1);
        }
    }
}

void apt_int::fixPaths()
{
    if(_mirrorPath.starts_with("$base_path"))
        _mirrorPath = _basePath + _mirrorPath.substr(10);
    if(_varPath.starts_with("$base_path"))
        _varPath = _basePath + _varPath.substr(10);
}

void apt_int::setLock()
{
    _lockfd = open((_varPath+"/apt-mirror.lock").c_str(),O_CREAT|O_RDONLY,0666);
     if (flock(_lockfd, LOCK_EX | LOCK_NB) != 0)
     {
         std::cerr << "Couldn't get lock on lock file, maybe apt-mirror is running?" << '\n';
         exit(1);
     }
}

void apt_int::releaseLock()
{
    if (flock(_lockfd, LOCK_UN)!=0 || close(_lockfd) != 0)
    {
        std::cerr << "Error Unlocking file" << "\n";
    }
    if(unlink((_varPath+"/apt-mirror.lock").c_str())!=0)
    {
        std::cerr << "Error deleting lock file" << "\n";
    }

}

int apt_int::main()
{
    readSourceFile();
    fixPaths();
    setLock();
    releaseLock();
    return 0;
}