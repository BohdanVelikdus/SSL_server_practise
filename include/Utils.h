#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include <optional>
#include <iostream>
#include <fstream>
#include <functional>

#ifdef _WIN32
    #include "windows.h"
    #include "psapi.h"
#else
    #include "sys/resource.h"
    #include "sys/sysinfo.h"
    #include "unistd.h"
    #include "sys/time.h"
    #include "unistd.h"
#endif

#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/err.h" 

struct CpuTimes {
    uint64_t idle, total;
};

inline CpuTimes getCpuTimes()
{
#ifdef _WIN32
    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);
    uint64_t idle = *(uint64_t*)&idleTime;
    uint64_t total = *(uint64_t*)&kernelTime + *(uint64_t*)&userTime;
    return { idle, total };    
#else
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
    FILE* file = fopen("/proc/stat", "r");
    if (!file) return {0, 0};

    fscanf(file, "cpu %lu %lu %lu %lu %lu %lu %lu %lu", 
        &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    fclose(file);

    uint64_t total = user + nice + system + idle + iowait + irq + softirq + steal;
    return { idle, total }  ;
#endif
}

inline double calculate_cpu_usage(const CpuTimes& prev, const CpuTimes& curr) {
    uint64_t total_diff = curr.total - prev.total;
    uint64_t idle_diff = curr.idle - prev.idle;
    // get the cpu load
    return (total_diff - idle_diff) * 100.0 / total_diff;
}

inline std::size_t get_memory_usage() {
    #ifdef _WIN32
        PROCESS_MEMORY_COUNTERS memInfo;
        GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
        return memInfo.WorkingSetSize / 1024; // KB
    #else
        std::ifstream file("/proc/self/status");
        std::string key;
        std::size_t value;
        while (file >> key >> value) {
            if (key == "VmRSS:") return value; // KB
        }
        return 0;
    #endif
}

inline std::size_t get_total_system_memory() {
    #ifdef _WIN32
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(memInfo);
        GlobalMemoryStatusEx(&memInfo);
        return memInfo.ullTotalPhys / 1024; // KB
    #else
        struct sysinfo memInfo;
        sysinfo(&memInfo);
        return memInfo.totalram * memInfo.mem_unit / 1024; // KB
    #endif
}


inline bool isValidFile(std::string path)
{
    std::filesystem::path path_ = std::filesystem::absolute(path);
    if(std::filesystem::exists(path) && std::filesystem::is_regular_file(path_))
    {
        return true;
    }
    return false;
}

inline std::optional<std::string> GetInputFromConsoleString()
{
    std::string input;
    std::cin >> input;
    if(std::cin.eof())
        return std::nullopt;
    return input;
}

inline int passwordCallbackUtils(char* buf, int size, int rwflag, void* userdata) {
    std::string* password = reinterpret_cast<std::string*>(userdata);
    std::size_t len = password->size();
    if (len > static_cast<std::size_t>(size)) {
        len = size - 1; // Ensure we don't exceed the buffer size
    }
    memcpy(buf, password->data(), len);
    buf[len] = '\0'; // Null-terminate the password
    return len;
}

inline bool certificatesMatches(std::string pathToKey, std::string password, std::string pathToCert)
{
    std::unique_ptr<BIO, std::function<void(BIO*)>> bioPrivateKey(BIO_new_file(pathToKey.c_str(), "r"), [](BIO* bio) { BIO_free(bio); });
    if (!bioPrivateKey) {
        std::cout << "Failed to create BIO for private key\n";
        return false;
    }

    std::unique_ptr<EVP_PKEY, std::function<void(EVP_PKEY*)>> pkey(nullptr, [](EVP_PKEY* pkey) { EVP_PKEY_free(pkey); });
    EVP_PKEY* k = PEM_read_bio_PrivateKey(bioPrivateKey.get(), nullptr, passwordCallbackUtils, reinterpret_cast<void*>(&password));
    if (k == nullptr) {
        std::cout << "Cannot read the private key\n";
        return false;
    } else {
        std::cout << "Read the private key\n";
        pkey.reset(k);
    }

    // Load the certificate
    std::unique_ptr<BIO, std::function<void(BIO*)>> bioX509(BIO_new_file(pathToCert.c_str(), "r"), [](BIO* bio) { BIO_free(bio); });
    if (!bioX509) {
        std::cout << "Failed to create BIO for certificate\n";
        return false;
    }

    std::unique_ptr<X509, std::function<void(X509*)>> certX509(nullptr, [](X509* x509) { X509_free(x509); });
    X509* cert = PEM_read_bio_X509(bioX509.get(), nullptr, nullptr, nullptr);
    if (cert == nullptr) {
        std::cout << "Cannot read the certificate\n";
        return false;
    } else {
        std::cout << "Read the certificate\n";
        certX509.reset(cert);
    }

    // Extract the public key from the certificate
    std::unique_ptr<EVP_PKEY, std::function<void(EVP_PKEY*)> > pub(X509_get_pubkey(certX509.get()), [](EVP_PKEY* key){ EVP_PKEY_free(key);});
    if (pub == nullptr) {
        std::cout << "Cannot extract the public key from the certificate\n";
        return false;
    } else {
        std::cout << "Extracted pub key from certificate\n";
    }

    // Compare the public keys
    int res = EVP_PKEY_eq(pub.get(), pkey.get());
    if (res == 1) {
        std::cout << "The certificate and private key match." << std::endl;
        return true;
    } else if (res == 0) {
        std::cerr << "The certificate and private key do not match." << std::endl;
        return false;
    } else {
        std::cout << "Error comparing public keys";
        return false;
    }
}