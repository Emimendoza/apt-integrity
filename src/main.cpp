#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

class apt_integrity
{
public:

private:

};

struct source
{
    std::string arch = "amd64";
    std::string source;
    std::string distro;
    std::vector<std::string> scopes;
};

std::vector<std::string> splitString(std::string str)
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

std::vector<source> readSourceFile()
{
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
                bool explicitArch = false;

                if (line.substr(4,6) == "[arch=")
                {
                    explicitArch = true;
                    currentSource.arch = line.substr(10, line.find(']')-10);
                }

            }
        }
        catch (...)
        {
            std::cerr << "Error Parsing line in source list:" << line << "\n";
        }
    }

}

int main() {
    std::cout << "Hello, World!" << std::endl;
    readSourceFile();
    return 0;
}
