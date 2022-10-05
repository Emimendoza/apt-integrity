#ifndef APT_INTEGRITY_APT_INTEGRITY_H
#define APT_INTEGRITY_APT_INTEGRITY_H

#include <vector>
#include <string>
#include <mutex>

namespace apt_integrity
{

    struct source
    {
        std::string arch = "amd64";
        std::string source;
        std::string distro;
        std::vector<std::string> scopes;
    };

    class apt_int{
    public:
        int main();
        // Args
        bool verbose = false;
        int threads = -1;
    private:
        // Static Variables
        static std::mutex _coutLock, _cerrLock;
        // Variables
        unsigned int _threads;
        int _lockfd;
        std::vector<source> _sources;
        std::string _mirrorPath = "$base_path/mirror";
        std::string _basePath = "/var/spool/apt-mirror";
        std::string _varPath = "$base_path/var";
        // Helper Functions
        static std::vector<std::string> splitString(const std::string& str);
        bool sha256(const std::string& path, const std::string& knownHash);
        // Main Functions
        void setArgs();
        void readSourceFile();
        void fixPaths();
        void setLock();
        void releaseLock();
    };
}

#endif //APT_INTEGRITY_APT_INTEGRITY_H
