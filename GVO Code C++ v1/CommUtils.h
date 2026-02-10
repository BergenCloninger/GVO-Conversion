#pragma once
#include <string>	
#include "OMS68SERMC.h"

bool InitCommUtils();
bool SendCommand(const std::string& cmd);
bool SendAndGetCommand(SCOMM_STRUCT* comm, const char* cmd, char* response, size_t respSize);
bool InitComm();

struct Coord {
    double RA;
    double Dec;
    double RASync;
    double DecSync;
    double RAGoto;
    double DecGoto;
};

namespace CommUtils {
    extern void* pndomem;
    bool InitSharedMem(const std::string& dllPath);
    void ShutdownSharedMem();
    Coord* GetCoordPtr();
}