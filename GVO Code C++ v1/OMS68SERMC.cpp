#include "OMS68SERMC.h"
#include <windows.h>
#include <iostream>
#include <string>

typedef long(__stdcall* InitOmsCommPort_t)(SCOMM_STRUCT*);
typedef long(__stdcall* SendString_t)(SCOMM_STRUCT*, char*);
typedef long(__stdcall* SendAndGetString_t)(SCOMM_STRUCT*, char*, char*);

SCOMM_STRUCT CommRecord = {};
HMODULE DLLHandle = nullptr;
InitOmsCommPort_t pInitOmsCommPort = nullptr;
SendString_t pSendString = nullptr;
SendAndGetString_t pSendAndGetString = nullptr;

bool LoadDLL(const std::string& dllPath) {
	DLLHandle = LoadLibraryA(dllPath.c_str());
	if (!DLLHandle) {
		DWORD err = GetLastError();
		std::cerr << "LoadLibrary failed for " << dllPath
		          << ", error code: " << err << std::endl;
		return false;
	}

	pInitOmsCommPort = reinterpret_cast<InitOmsCommPort_t>(
		GetProcAddress(DLLHandle, "InitOmsCommPort")
	);

	pSendString = reinterpret_cast<SendString_t>(
		GetProcAddress(DLLHandle, "SendString")
	);

	pSendAndGetString = reinterpret_cast<SendAndGetString_t>(
		GetProcAddress(DLLHandle, "SendAndGetString")
	);

	if (!pInitOmsCommPort || !pSendString || !pSendAndGetString) {
		std::cerr << "Error: Could not get function addresses!" << std::endl;
		FreeLibrary(DLLHandle);
		DLLHandle = nullptr;
		return false;
	}

	CommRecord.BaudRate = 9600;
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
		pSendAndGetString = nullptr;
	}
}