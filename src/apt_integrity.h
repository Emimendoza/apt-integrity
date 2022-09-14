#ifndef APT_INTEGRITY_APT_INTEGRITY_H
#define APT_INTEGRITY_APT_INTEGRITY_H

#include <vector>
#include <string>

namespace apt_integrity{

    struct source
    {
        std::string arch = "amd64";
        std::string source;
        std::string distro;
        std::vector<std::string> scopes;
    };

    class apt_int {
    public:
        int main();
    private:
        // Variables
        std::vector<source> _sources;
        // Functions
        static std::vector<std::string> splitString(const std::string& str);
        void readSourceFile();
    };
}



#endif //APT_INTEGRITY_APT_INTEGRITY_H
