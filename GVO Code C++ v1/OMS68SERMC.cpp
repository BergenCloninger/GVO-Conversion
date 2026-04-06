#include "OMS68SERMC.h"
#include <windows.h>
#include <iostream>

// Function pointers
typedef long(__stdcall* InitOmsCommPort_t)(SCOMM_STRUCT*);
typedef long(__stdcall* SendString_t)(SCOMM_STRUCT*, char*);

SCOMM_STRUCT CommRecord = {};
HMODULE DLLHandle = nullptr;
InitOmsCommPort_t pInitOmsCommPort = nullptr;
SendString_t pSendString = nullptr;
SendAndGetString_t pSendAndGetString = nullptr;

bool LoadDLL() {
    // Load the DLL once
    DLLHandle = LoadLibraryA("E:\\VSCode Projects\\Observatory Code Conversion\\GVO-Conversion\\GVO Code C++ v1\\68SERMC.DLL"); //Should load dynamically from local directory
    if (!DLLHandle) {
        DWORD err = GetLastError();
        std::cerr << "LoadLibrary failed, error code: " << err << std::endl;
        return false;
    }

    // Get function pointers
    pInitOmsCommPort = reinterpret_cast<InitOmsCommPort_t>(
        GetProcAddress(DLLHandle, "InitOmsCommPort")
    );
    
    pSendString = reinterpret_cast<SendString_t>(
        GetProcAddress(DLLHandle, "SendString")
    );

    if (!pInitOmsCommPort || !pSendString) {
        std::cerr << "Error: Could not get function addresses!" << std::endl;
        FreeLibrary(DLLHandle);
        DLLHandle = nullptr;
        return false;
    }

    // Initialize CommRecord defaults
    CommRecord.BaudRate = 9600;
    CommRecord.CommPortNumber = 1;
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

void UnloadDLL() {
    if (DLLHandle) {
        FreeLibrary(DLLHandle);
        DLLHandle = nullptr;
        pInitOmsCommPort = nullptr;
        pSendString = nullptr;
    }
}