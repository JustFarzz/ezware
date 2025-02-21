#pragma once

#include <Windows.h>
#include <vector>

#ifdef _WIN32
#ifdef BUILDING_DLL
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif
#else
#define DLLEXPORT
#endif

class DLLEXPORT MemoryMonitor {
private:
    HANDLE processHandle;
    std::vector<float> memoryHistory;
    static const int MAX_SAMPLES = 100;
    SIZE_T peakMemoryUsage;
    bool isMonitoring;
    bool hasHistory;

    DWORD GetProcessIdByNameAndPath(const wchar_t* processName, const char* processPath);

public:
    MemoryMonitor();
    ~MemoryMonitor();

    bool Initialize(const wchar_t* processName, const char* processPath);
    float GetMemoryUsage();
    const std::vector<float>& GetMemoryHistory() const { return memoryHistory; }
    SIZE_T GetPeakMemoryUsage() const { return peakMemoryUsage; }
    bool HasHistory() const { return hasHistory; }
    bool IsProcessRunning();
    bool IsMonitoring() const { return isMonitoring; }
    void InitializeEmptyHistory();
};

extern MemoryMonitor g_memoryMonitor;
extern bool g_memoryMonitorInitialized;