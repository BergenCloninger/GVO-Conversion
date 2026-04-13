#include <windows.h>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "CommUtils.h"
#include "OMS68SERMC.h"

namespace {
	bool g_commBusy = false;

	struct BusyGuard {
		bool& flag;
		explicit BusyGuard(bool& f) : flag(f) {
			flag = true;
		}
		~BusyGuard() {
			flag = false;
		}
	};
}

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

	if (!pSendString) {
		std::cerr << "Failed to get SendString pointer\n";
	}
	if (!pSendAndGetString) {
		std::cerr << "Failed to get SendAndGetString pointer\n";
	}

	return pSendString != nullptr && pSendAndGetString != nullptr;
}

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

// Wrapper "send command" function for DLL
bool SendCommand(const std::string& cmd) {
	if (g_commBusy) {
		return false;
	}

	if (!pSendString) {
		return false;
	}

	if (cmd.empty()) {
		return false;
	}

	BusyGuard guard(g_commBusy);

	//std::cout << "SEND: [" << cmd << "]\n";

	std::vector<char> buffer(cmd.begin(), cmd.end());
	buffer.push_back('\0');

	long result = pSendString(&CommRecord, buffer.data());

	if (result != SUCCESS) {
		return false;
	}

	return true;
}

// Send a command and get a response string
bool SendAndGetCommand(SCOMM_STRUCT* comm, const char* cmd, char* response, size_t respSize) {
	if (g_commBusy) {
		if (response && respSize > 0) {
			response[0] = '\0';
		}
		return false;
	}

	if (!pSendAndGetString) {
		if (response && respSize > 0) {
			response[0] = '\0';
		}
		return false;
	}

	if (!comm) {
		if (response && respSize > 0) {
			response[0] = '\0';
		}
		return false;
	}

	if (!cmd) {
		if (response && respSize > 0) {
			response[0] = '\0';
		}
		return false;
	}

	if (!response) {
		return false;
	}

	if (respSize == 0) {
		return false;
	}

	BusyGuard guard(g_commBusy);

	response[0] = '\0';

	//std::cout << "SEND: [" << cmd << "]\n";

	std::vector<char> cmdBuffer(cmd, cmd + std::strlen(cmd));
	cmdBuffer.push_back('\0');

	long result = pSendAndGetString(comm, cmdBuffer.data(), response);

	response[respSize - 1] = '\0';

	if (result != SUCCESS) {
		response[0] = '\0';
		return false;
	}

	return true;
}

void __stdcall DLLWork(int value) {
	std::cout << "[Callback] value = " << value << std::endl;
}

// Shared memory helpers
namespace CommUtils {
	static HMODULE hDLL = nullptr;

	using getndomem_t = void* (__stdcall*)();
	using setcallback_t = void (__stdcall*)(void (__stdcall*)(int));

	static getndomem_t getndomem = nullptr;
	static setcallback_t setcallback = nullptr;

	void* pndomem = nullptr;

	bool InitSharedMem(const std::string& dllPath) {
		hDLL = LoadLibraryA(dllPath.c_str());
		if (!hDLL) {
			std::cerr << "Failed to load DLL\n";
			return false;
		}

		FARPROC cbProc = GetProcAddress(hDLL, MAKEINTRESOURCEA(41));
		if (!cbProc) {
			std::cerr << "Failed to load setcallback (ordinal 41)\n";
		} else {
			setcallback = reinterpret_cast<setcallback_t>(cbProc);
			setcallback(DLLWork);
			std::cout << "setcallback registered\n";
		}

		FARPROC memProc = GetProcAddress(hDLL, MAKEINTRESOURCEA(40));
		if (!memProc) {
			std::cerr << "Failed to find ordinal 40\n";
			return false;
		}

		getndomem = reinterpret_cast<getndomem_t>(memProc);
		pndomem = getndomem();

		if (!pndomem) {
			std::cerr << "getndomem returned null\n";
			return false;
		}

		std::cout << "Shared memory initialized at " << pndomem << "\n";
		return true;
	}

	Coord* GetCoordPtr() {
		if (!pndomem) {
			std::cerr << "Shared memory not initialized\n";
			return nullptr;
		}
		return reinterpret_cast<Coord*>(pndomem);
	}

	void ShutdownSharedMem() {
		pndomem = nullptr;
		getndomem = nullptr;
		setcallback = nullptr;

		if (hDLL) {
			FreeLibrary(hDLL);
			hDLL = nullptr;
		}
	}
}