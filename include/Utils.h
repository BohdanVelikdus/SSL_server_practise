#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include <optional>
#include <iostream>
#include <fstream>

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