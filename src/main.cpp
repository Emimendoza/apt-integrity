#include "apt_integrity.h"
#include <iostream>

int main(int argc, char *argv[]) {
    apt_integrity::apt_int main;
    std::string arg;
    for (int i = 1; i<argc; i++)
    {
        arg = argv[i];
        if(arg=="-h" or arg=="--help")
        {
            std::cout << "apt-integrity\n"
                         "Usage: apt-integrity [options]\n"
                         "\n"
                         "Options:\n"
                         "\t-t or --threads: Specify the number of threads to use in the download/verification process (-1 for auto, 0 for whatever is in mirror.list). Default:-1\n"
                         "\t-v or --verbose: Know exactly what is being done right now\n"
                         "\t-h or --help: display this page\n"
                         "\n"
                         "apt-integrity is a command to help you validate any apt-mirror mirror of .deb repositories\n";
            return 0;
        }
        else if(arg=="-v" or arg=="--verbose")
        {
            main.verbose = true;
        }
        else if(arg=="-t" or arg=="--threads")
        {
           if(i+1>=argc)
           {
               std::cerr << arg << " argument specified but no thread count given\n";
           }
           i++;
           main.threads = std::stoi(argv[i]);
        }
        else
        {
            std::cerr << "Unknown argument:" + arg + "\n";
            return 1;
        }
    }
    return main.main();
}
