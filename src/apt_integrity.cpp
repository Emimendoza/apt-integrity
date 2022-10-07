#include "apt_integrity.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/file.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <iomanip>
#include <thread>

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
void apt_int::print(const std::string& text)
{
    _coutLock.lock();
    std::cout << text;
    _coutLock.unlock();
}
void apt_int::println(const std::string& text)
{
    print(text + "\n");
}
void apt_int::printe(const std::string& text)
{
    _cerrLock.lock();
    std::cerr << text;
    _cerrLock.unlock();
}
void apt_int::printeln(const std::string& text)
{
    printe(text+"\n");
}

bool apt_int::sha256(const std::string &path,const std::string& knownHash, const unsigned int &currThread)
{
    // Open File
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file)
    {
        if(verbose)
            println("Cannot open file:" +path);
        return true;
    }
    // Get CTX ready
    EVP_MD_CTX_reset(threadCtx[currThread].ctx);
    EVP_DigestInit_ex(threadCtx[currThread].ctx, EVP_sha256(), nullptr);
    // Get Hash
    while(file)
    {
        file.read(threadCtx[currThread].fileBuffer, 4096);
        EVP_DigestUpdate(threadCtx[currThread].ctx, threadCtx[currThread].fileBuffer, file.gcount());
    }
    // Finalize
    EVP_DigestFinal_ex(threadCtx[currThread].ctx,threadCtx[currThread].hash, &size);
    file.close();
    // Get hex string
    std::stringstream ss;
    ss << std::setfill('0') << std::hex;
    for(int i=0; i<size; ++i)
        ss << std::setw(2) << (unsigned short)threadCtx[currThread].hash[i];
    // Compare with expected hash
    if(ss.str()==knownHash)
    {
        if (verbose)
            println(path + ": OK");
        return false;
    }
    if(verbose)
        println(": BAD Expected:" + knownHash + " ACTUAL:" + ss.str());
    return true;
}
// Main Functions
void apt_int::setArgs()
{
    if (threads <0)
    {
        _threads = std::thread::hardware_concurrency();
        if(_threads == 0)
        {
            std::cerr << "Unable to detect cpu count. Using thread count specified in mirror file \n";
            threads = 0;
        }
        return;
    }
    _threads = threads;
}

void apt_int::readSourceFile()
{
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
                if (tokens[1] == "nthreads" and threads == 0)
                    _threads = std::stoi(tokens[2]);
            }
        }
        catch (...)
        {
            std::cerr << "Error Parsing line in source list:" << line << "\n";
            exit(1);
        }
    }
    sourceList.close();
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
         printeln("Couldn't get lock on lock file, maybe apt-mirror is running?");
         exit(1);
     }
}

void apt_int::initThreads()
{
    threadCtx = (thread_ctx *) malloc(sizeof(thread_ctx)*_threads);
    for(int i = 0 ; i< _threads; i++)
    {
        threadCtx[i].ctx = EVP_MD_CTX_new();
        threadCtx[i].hash = (unsigned char *)OPENSSL_malloc(size);
    }
}

void apt_int::freeThreads()
{
    for(int i = 0 ; i< _threads; i++)
    {
        EVP_MD_CTX_free(threadCtx[i].ctx);
        free(threadCtx[i].hash);
    }
    free(threadCtx);
}

void apt_int::releaseLock()
{
    if (flock(_lockfd, LOCK_UN)!=0 || close(_lockfd) != 0)
    {
        printeln("Error Unlocking file");
    }
    if(unlink((_varPath+"/apt-mirror.lock").c_str())!=0)
    {
        printeln( "Error deleting lock file");
    }
}

int apt_int::main()
{
    setArgs();
    readSourceFile();
    fixPaths();
    setLock();
    initThreads();
    freeThreads();
    releaseLock();
    return 0;
}