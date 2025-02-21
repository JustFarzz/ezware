#define BUILDING_DLL
#include "cpu_process.h"
#include <Psapi.h>
#include <iostream>

ProcessMonitor g_processMonitor;
bool g_monitorInitialized = false;

void ProcessMonitor::InitializeEmptyHistory()
{
    history.clear();
    // Isi dengan 100 data nol
    for (int i = 0; i < MAX_SAMPLES; i++) {
        history.push_back(0.0f);
    }
    hasHistory = true;
}

ProcessMonitor::ProcessMonitor() :
    processHandle(NULL),
    isMonitoring(false),
    hasHistory(false)
{
    history.reserve(MAX_SAMPLES);

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    numProcessors = sysInfo.dwNumberOfProcessors;

    lastCPU.QuadPart = 0;
    lastSysCPU.QuadPart = 0;
    lastUserCPU.QuadPart = 0;

    InitializeEmptyHistory();
}

ProcessMonitor::~ProcessMonitor()
{
    if (processHandle)
    {
        CloseHandle(processHandle);
    }
}

bool ProcessMonitor::Initialize(const wchar_t* processName, const char* processPath)
{
    DWORD processId = GetProcessIdByNameAndPath(processName, processPath);
    if (processId == 0)
    {
        std::cout << "Process not found: " << processPath << std::endl;
        isMonitoring = false;
        return true;
    }

    processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (processHandle == NULL)
    {
        std::cout << "Failed to open process. Error: " << GetLastError() << std::endl;
        isMonitoring = false;
        return true;
    }

    FILETIME ftime, fsys, fuser;
    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));

    if (!GetProcessTimes(processHandle, &ftime, &ftime, &fsys, &fuser))
    {
        std::cout << "Failed to get process times. Error: " << GetLastError() << std::endl;
        CloseHandle(processHandle);
        isMonitoring = false;
        return true;
    }

    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

    isMonitoring = true;
    return true;
}

float ProcessMonitor::GetCPUUsage()
{
    if (!IsProcessRunning())
    {
        isMonitoring = false;
        if (hasHistory && !history.empty())
        {
            return history.back();
        }
        return 0.0f;
    }

    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    if (!GetProcessTimes(processHandle, &ftime, &ftime, &fsys, &fuser))
    {
        isMonitoring = false;
        if (hasHistory && !history.empty())
        {
            return history.back();
        }
        return 0.0f;
    }

    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));

    float percent = 0.0f;
    if (now.QuadPart - lastCPU.QuadPart > 0)
    {
        percent = (sys.QuadPart - lastSysCPU.QuadPart) +
            (user.QuadPart - lastUserCPU.QuadPart);
        percent /= (now.QuadPart - lastCPU.QuadPart);
        percent /= numProcessors;
    }

    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;

    percent = max(0.0f, min(1.0f, percent));

    if (history.size() >= MAX_SAMPLES)
    {
        history.erase(history.begin());
    }
    history.push_back(percent);
    hasHistory = true;

    return percent;
}

bool ProcessMonitor::IsProcessRunning()
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

DWORD ProcessMonitor::GetProcessIdByNameAndPath(const wchar_t* processName, const char* processPath)
{
    DWORD processes[1024], needed;
    if (!EnumProcesses(processes, sizeof(processes), &needed))
    {
        std::cout << "Failed to enumerate processes. Error: " << GetLastError() << std::endl;
        return 0;
    }

    DWORD count = needed / sizeof(DWORD);
    for (DWORD i = 0; i < count; i++) {
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
                        std::cout << "Found process: " << szProcessPath << std::endl;
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