#define BUILDING_DLL
#include "memory_process.h"
#include <Psapi.h>
#include <iostream>

MemoryMonitor g_memoryMonitor;
bool g_memoryMonitorInitialized = false;

MemoryMonitor::MemoryMonitor() :
    processHandle(NULL),
    isMonitoring(false),
    hasHistory(false),
    peakMemoryUsage(0)
{
    memoryHistory.reserve(MAX_SAMPLES);
    InitializeEmptyHistory();
}

MemoryMonitor::~MemoryMonitor()
{
    if (processHandle)
    {
        CloseHandle(processHandle);
    }
}

void MemoryMonitor::InitializeEmptyHistory()
{
    memoryHistory.clear();
    for (int i = 0; i < MAX_SAMPLES; i++) {
        memoryHistory.push_back(0.0f);
    }
    hasHistory = true;
}

bool MemoryMonitor::Initialize(const wchar_t* processName, const char* processPath)
{
    DWORD processId = GetProcessIdByNameAndPath(processName, processPath);
    if (processId == 0)
    {
        std::cout << "Process not found for memory monitoring: " << processPath << std::endl;
        isMonitoring = false;
        return true;
    }

    processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (processHandle == NULL)
    {
        std::cout << "Failed to open process for memory monitoring. Error: " << GetLastError() << std::endl;
        isMonitoring = false;
        return true;
    }

    isMonitoring = true;
    return true;
}

float MemoryMonitor::GetMemoryUsage()
{
    if (!IsProcessRunning())
    {
        isMonitoring = false;
        if (hasHistory && !memoryHistory.empty())
        {
            return memoryHistory.back();
        }
        return 0.0f;
    }

    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(processHandle, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
    {
        float memoryUsageMB = static_cast<float>(pmc.WorkingSetSize) / (1024.0f * 1024.0f);
        peakMemoryUsage = max(peakMemoryUsage, pmc.WorkingSetSize);

        if (memoryHistory.size() >= MAX_SAMPLES)
        {
            memoryHistory.erase(memoryHistory.begin());
        }
        memoryHistory.push_back(memoryUsageMB);

        return memoryUsageMB;
    }

    return 0.0f;
}

bool MemoryMonitor::IsProcessRunning()
{
    if (processHandle == NULL)
    {
        return false;
    }

    DWORD exitCode;
    if (!GetExitCodeProcess(processHandle, &exitCode))
    {
        return false;
    }

    return exitCode == STILL_ACTIVE;
}

DWORD MemoryMonitor::GetProcessIdByNameAndPath(const wchar_t* processName, const char* processPath)
{
    DWORD processes[1024], needed;
    if (!EnumProcesses(processes, sizeof(processes), &needed))
    {
        std::cout << "Failed to enumerate processes for memory monitoring. Error: " << GetLastError() << std::endl;
        return 0;
    }

    DWORD count = needed / sizeof(DWORD);
    for (DWORD i = 0; i < count; i++)
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
        if (hProcess)
        {
            wchar_t szProcessName[MAX_PATH];
            char szProcessPath[MAX_PATH];
            HMODULE hMod;
            DWORD cbNeeded;

            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
            {
                GetModuleBaseNameW(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(wchar_t));
                GetModuleFileNameExA(hProcess, hMod, szProcessPath, MAX_PATH);

                if (_wcsicmp(szProcessName, processName) == 0)
                {
                    if (strstr(szProcessPath, processPath) != nullptr)
                    {
                        std::cout << "Found process for memory monitoring: " << szProcessPath << std::endl;
                        CloseHandle(hProcess);
                        return processes[i];
                    }
                }
            }
            CloseHandle(hProcess);
        }
    }
    return 0;
}