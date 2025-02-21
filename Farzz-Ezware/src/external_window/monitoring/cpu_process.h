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

class DLLEXPORT ProcessMonitor
{
private:
    HANDLE processHandle;
    std::vector<float> history;
    static const int MAX_SAMPLES = 100;

    ULARGE_INTEGER lastCPU;
    ULARGE_INTEGER lastSysCPU;
    ULARGE_INTEGER lastUserCPU;
    DWORD numProcessors;

    bool isMonitoring;
    bool hasHistory;

    DWORD GetProcessIdByNameAndPath(const wchar_t* processName, const char* processPath);

public:
    ProcessMonitor();
    ~ProcessMonitor();

    bool Initialize(const wchar_t* processName, const char* processPath);
    float GetCPUUsage();
    const std::vector<float>& GetHistory() const { return history; }
    bool HasHistory() const { return hasHistory; }
    bool IsProcessRunning();
    bool IsMonitoring() const { return isMonitoring; }

    void InitializeEmptyHistory();
};

extern ProcessMonitor g_processMonitor;
extern bool g_monitorInitialized;