#include "OMS68SERMC.h"
#include <windows.h>
#include <iostream>

// Function pointers
typedef long(__stdcall* InitOmsCommPort_t)(SCOMM_STRUCT*);
typedef long(__stdcall* SendString_t)(SCOMM_STRUCT*, char*);

SCOMM_STRUCT CommRecord = {};
static HMODULE DLLHandle = nullptr;
static InitOmsCommPort_t pInitOmsCommPort = nullptr;
static SendString_t pSendString = nullptr;

bool LoadDLL()
{
    DLLHandle = LoadLibrary(L"68SERMC.DLL");
    if (!DLLHandle) {
        std::cerr << "Error: 68SERMC.DLL could not be loaded!" << std::endl;
        return false;
    }

    // Load functions
    pInitOmsCommPort = reinterpret_cast<InitOmsCommPort_t>(
        GetProcAddress(DLLHandle, "InitOmsCommPort"));
    pSendString = reinterpret_cast<SendString_t>(
        GetProcAddress(DLLHandle, "SendString"));

    if (!pInitOmsCommPort || !pSendString) {
        std::cerr << "Error: Could not get function addresses!" << std::endl;
        FreeLibrary(DLLHandle);
        DLLHandle = nullptr;
        return false;
    }

    // Initialize CommRecord defaults
    CommRecord.BaudRate = 9600;
    CommRecord.CommPortNumber = 1;  // default COM port
    CommRecord.hComm = nullptr;
    CommRecord.AxisDoneFlags = 0;
    CommRecord.GlobalDone = false;
    CommRecord.Overtravel = false;
    CommRecord.CmdError = false;
    CommRecord.Slip = false;
    CommRecord.Mode = 0;
    CommRecord.TimeLimit = 0;
    CommRecord.LF_Count = 0;
    CommRecord.Timer = 0;

    return true;
}

void UnloadDLL()
{
    if (DLLHandle) {
        FreeLibrary(DLLHandle);
        DLLHandle = nullptr;
        pInitOmsCommPort = nullptr;
        pSendString = nullptr;
    }
}

bool InitComm() {
    if (!pInitOmsCommPort)
        return false;
    long result = pInitOmsCommPort(&CommRecord);
    if (result != SUCCESS) {
        std::cerr << "InitOmsCommPort failed with code " << result << std::endl;
        return false;
    }
    return true;
}

bool SendCommand(const char* cmd) {
    if (!pSendString)
        return false;

    char buffer[256];
    strncpy_s(buffer, cmd, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = 0;

    long result = pSendString(&CommRecord, buffer);
    if (result != SUCCESS) {
        std::cerr << "SendString failed with code " << result << std::endl;
        return false;
    }
    return true;
}
