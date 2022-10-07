#ifndef APT_INTEGRITY_APT_INTEGRITY_H
#define APT_INTEGRITY_APT_INTEGRITY_H

#include <vector>
#include <string>
#include <mutex>
#include <openssl/evp.h>

namespace apt_integrity
{
    struct thread_ctx
    {
        EVP_MD_CTX *ctx;
        char fileBuffer[4096];
        unsigned char *hash;
    };
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
        std::mutex _coutLock, _cerrLock;
        thread_ctx *threadCtx;
        // Variables
        unsigned int size = EVP_MD_size(EVP_sha256());
        unsigned int _threads;
        int _lockfd;
        std::vector<source> _sources;
        std::string _mirrorPath = "$base_path/mirror";
        std::string _basePath = "/var/spool/apt-mirror";
        std::string _varPath = "$base_path/var";
        // Helper Functions
        static std::vector<std::string> splitString(const std::string& str);
        void print(const std::string& text);
        void printe(const std::string& text);
        void println(const std::string& text);
        void printeln(const std::string& text);
        [[nodiscard]] bool sha256(const std::string& path, const std::string& knownHash, const unsigned int &currThread);
        // Main Functions
        void setArgs();
        void readSourceFile();
        void fixPaths();
        void setLock();
        void initThreads();
        void freeThreads();
        void releaseLock();
    };
}

#endif //APT_INTEGRITY_APT_INTEGRITY_H
