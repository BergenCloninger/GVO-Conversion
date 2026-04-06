#include <iostream>
#include <string>
#include <windows.h>
#include "CommUtils.h"
#include "OMS68SERMC.h"

// ---------------------------------------------------------------------------
// Initialize SendString function from DLL
bool InitCommUtils() {
	if (!DLLHandle) {
		std::cerr << "DLLHandle is null!\n";
		return false;
	}

	pSendString = reinterpret_cast<SendString_t>(
		GetProcAddress(DLLHandle, "SendString")
	);

	pSendAndGetString = reinterpret_cast<SendAndGetString_t>(
		GetProcAddress(DLLHandle, "SendAndGetString")
	);

	if (!pSendString) std::cerr << "Failed to get SendString pointer\n";
	if (!pSendAndGetString) std::cerr << "Failed to get SendAndGetString pointer\n";

	return pSendString != nullptr && pSendAndGetString != nullptr;
}

// ---------------------------------------------------------------------------
// Initialize the OMS communication port
bool InitComm() {
	if (!pInitOmsCommPort) {
		std::cerr << "InitOmsCommPort function pointer is null!\n";
		return false;
	}

	long result = pInitOmsCommPort(&CommRecord);

	if (result != SUCCESS) {
		std::cerr << "InitOmsCommPort failed with error code: " << result << "\n";

		switch (result) {
		case INVALID_PORT: std::cerr << " -> INVALID_PORT\n"; break;
		case INVALID_BAUD_RATE: std::cerr << " -> INVALID_BAUD_RATE\n"; break;
		case PORT_NOT_AVAILABLE: std::cerr << " -> PORT_NOT_AVAILABLE\n"; break;
		case COMM_TIMEOUT_ERROR: std::cerr << " -> COMM_TIMEOUT_ERROR\n"; break;
		case GET_COMM_STATE_ERROR: std::cerr << " -> GET_COMM_STATE_ERROR\n"; break;
		case SET_COMM_STATE_ERROR: std::cerr << " -> SET_COMM_STATE_ERROR\n"; break;
		case COMM_WRITE_ERROR: std::cerr << " -> COMM_WRITE_ERROR\n"; break;
		case COMM_READ_ERROR: std::cerr << " -> COMM_READ_ERROR\n"; break;
		case SERVO_READ_ERROR: std::cerr << " -> SERVO_READ_ERROR\n"; break;
		case COMM_IO_TIMEOUT: std::cerr << " -> COMM_IO_TIMEOUT\n"; break;
		default: std::cerr << " -> Unknown error\n"; break;
		}
		return false;
	}

	std::cout << "InitOmsCommPort succeeded.\n";
	return true;
}

// ---------------------------------------------------------------------------
// Send a command string
bool SendCommand(const std::string& cmd) {
	if (!pSendString) return false;

	char buffer[256];
	strncpy_s(buffer, cmd.c_str(), sizeof(buffer) - 1);
	buffer[sizeof(buffer) - 1] = 0;

	long result = pSendString(&CommRecord, buffer);
	if (result != SUCCESS) {
		std::cerr << "SendString failed with code " << result << std::endl;
		return false;
	}
	return true;
}

// Send a command and get a response string
bool SendAndGetCommand(SCOMM_STRUCT* comm, const char* cmd, char* response, size_t respSize) {
	if (!pSendAndGetString) return false;
	if (!comm || !cmd || !response || respSize == 0) return false;

	long result = pSendAndGetString(comm, const_cast<char*>(cmd), response);
	if (result != SUCCESS) {
		std::cerr << "SendAndGetString failed with code " << result << std::endl;
		if (respSize > 0) response[0] = 0;
		return false;
	}

	// Ensure null termination
	response[respSize - 1] = 0;
	return true;
}

// Shared memory helpers
namespace CommUtils {
	static HMODULE hDLL = nullptr;
	using getndomem_t = void* (__stdcall*)();
	static getndomem_t getndomem = nullptr;
	void* pndomem = nullptr;

	bool InitSharedMem(const std::string& dllPath) {
		hDLL = LoadLibraryA(dllPath.c_str());
		if (!hDLL) {
			std::cerr << "Failed to load DLL\n";
			return false;
		}

		getndomem = reinterpret_cast<getndomem_t>(GetProcAddress(hDLL, MAKEINTRESOURCEA(40)));
		if (!getndomem) {
			std::cerr << "Failed to find getndomem in DLL\n";
			FreeLibrary(hDLL);
			hDLL = nullptr;
			return false;
		}

		pndomem = getndomem();
		return pndomem != nullptr;
	}

	Coord* GetCoordPtr() {
		if (!pndomem) {
			std::cerr << "Shared memory not initialized (pndomem is null)\n";
			return nullptr;
		}
		return reinterpret_cast<Coord*>(pndomem);
	}

	void ShutdownSharedMem() {
		pndomem = nullptr;
		getndomem = nullptr;
		if (hDLL) {
			FreeLibrary(hDLL);
			hDLL = nullptr;
		}
	}
}
