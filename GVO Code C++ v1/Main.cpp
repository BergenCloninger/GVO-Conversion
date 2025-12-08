#include <iostream>
#include <string>
#include <cmath>
#include <cstdint>
#include <functional>
#include "OMS68SERMC.h"
#include <windows.h>
#include "CommUtils.h"
#include "TimerUnit.h"

using TeleAPICallBack = std::function<void(int)>;

enum class StateVar {
    Off,
    Tracking,
    CorrectingE,
    CorrectingW,
    CorrectingN,
    CorrectingS,
    Slewing
};

double TrkRate;
std::string xvlslew, yvlslew, xvl5inch, yvl5inch;
std::string xvl, xac, xacmax, xvlmax, yvl, yac, yacmax, yvlmax;
double RAFact, DECFACT;
int DecBack;
double C_Lat, C_Long;
int yPole = 1;
bool LastDecNorth = true;
bool SlewSelect = true;

bool EastOfMeridian = true;
bool LookingEast = true;
bool TargetEastOfMeridian = true;

uint8_t keystroke, pcxdataout, status;
std::string name, commandBuffer;
uint8_t halfSecondCounter;
int quadrant, targetQuadrant;
bool movingRA, movingDEC;
uint8_t io;
bool EastPushed, WestPushed, NorthPushed, SouthPushed, ManualPushed;
double SidTimeFract, SidTime, RANow, DECNow, azimuth;
int RAHr, RAMin, RASec;
int DECDeg, DECMin, DECSec;
int altdeg, altmin, altsec, azdeg, azmin, azsec;
double RaTarget, DecTarget, Meridian, EastHor, WestHor;
double EastHA, WestHA, NorthHA, SouthHA, A, H;
double RaPos, decPos, tdecfact;
bool Parkit, NorthofZenith, NoPassword;
char Response[256];
// ------------------------- DLL Function Pointers -------------------------
void PrintLastError() {
    DWORD err = GetLastError();
    LPVOID msgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&msgBuf,
        0, nullptr);
    std::cerr << "LoadLibrary failed: " << (char*)msgBuf << std::endl;
    LocalFree(msgBuf);
}

// ------------------------- Telescope Functions -------------------------
void loadparams() {
    tdecfact = ((360.0 / 338) * 3600) / 50000;
    tdecfact = 1 / tdecfact * 3600;

    TrkRate = 500.6;
    xvlslew = "75000";
    yvlslew = "50000";
    xvl5inch = "2000";
    yvl5inch = "2000";
    xvl = "10000";
    xac = "35000";
    yvl = "5000";
    yac = "25000";
    yacmax = "25000";
    yvlmax = "100000";
    xacmax = "35000";
    xvlmax = "75000";
}

// ------------------------- Jog Commands -------------------------
void JogNorthCommand() {
    std::string CmdStr;
    if (SlewSelect) {
        CmdStr = (yPole >= 0 ? "AY JG+" : "AY JG-") + yvl + ";";
    } else {
        int i = static_cast<int>(std::round(TrkRate / 4));
        CmdStr = (yPole >= 0 ? "AY JG" : "AY JG-") + std::to_string(i) + ";";
    }
    SendCommand(CmdStr);
}

void StopNorthCommand() { SendCommand("AY ST;"); }

void JogSouthCommand() {
    std::string CmdStr;
    if (SlewSelect) {
        CmdStr = (yPole >= 0 ? "AY JG-" : "AY JG+") + yvl + ";";
    } else {
        int i = static_cast<int>(std::round(TrkRate / 4));
        CmdStr = (yPole >= 0 ? "AY JG-" : "AY JG") + std::to_string(i) + ";";
    }
    SendCommand(CmdStr);
}

void StopSouthCommand() { SendCommand("AY ST;"); }

void JogEastCommand() {
    std::string CmdStr;
    if (SlewSelect) {
        CmdStr = "AX JG0; AX JG-" + xvl + ";";
    } else {
        int i = static_cast<int>(std::round(TrkRate / 2));
        CmdStr = "AX JG" + std::to_string(i) + ";";
    }
    SendCommand(CmdStr);
}

void StopEastCommand() { SendCommand("AX ST;"); }

void JogWestCommand() {
    std::string CmdStr;
    if (SlewSelect) {
        CmdStr = "AX JG0; JG" + xvl + ";";
    } else {
        int i = static_cast<int>(std::round(TrkRate + (TrkRate / 2)));
        CmdStr = "AX JG" + std::to_string(i) + ";";
    }
    SendCommand(CmdStr);
}

void StopWestCommand() { SendCommand("AX ST;"); }

void StopAll() {
    SendCommand("AA ST;");
}

void Park() {
    Parkit = true;
}

void ManualControlMenu() {
    while (true) {
        std::cout << "\n=== Telescope Manual Control Menu ===\n";
        std::cout << "1. Jog North\n";
        std::cout << "2. Jog South\n";
        std::cout << "3. Jog East\n";
        std::cout << "4. Jog West\n";
        std::cout << "5. Stop All Axes\n";
        std::cout << "6. Park Telescope\n";
        std::cout << "7. Connect to TheSky\n";
        std::cout << "0. Exit Manual Control\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Jogging North..." << std::endl;
                JogNorthCommand();
                break;
            case 2:
                std::cout << "Jogging South..." << std::endl;
                JogSouthCommand();
                break;
            case 3:
                std::cout << "Jogging East..." << std::endl;
                JogEastCommand();
                break;
            case 4:
                std::cout << "Jogging West..." << std::endl;
                JogWestCommand();
                break;
            case 5:
                std::cout << "Stopping all axes..." << std::endl;
                StopAll();
                break;
            case 6:
                std::cout << "Parking telescope..." << std::endl;
                Park();
                break;
            case 7:
                std::cout << "Connecting telescope..." << std::endl;
                TimerUpdate();
                break;
            case 0:
                std::cout << "Exiting manual control..." << std::endl;
                return;
            default:
                std::cout << "Invalid choice, try again." << std::endl;
        }
    }
}

int main() {
    if (!CommUtils::InitSharedMem("E:\\VSCode Projects\\Observatory Code Conversion\\GVO-Conversion\\GVO Code C++ v1\\teleapi.dll")) {
        std::cerr << "Failed to initialize shared memory.\n";
        return 1;
    }

    if (!LoadDLL()) return 1;

    if (!InitComm()) {
        std::cerr << "Failed to initialize telescope." << std::endl;
        return 1;
    } else {
        std::cout << "Connected to device on port " << CommRecord.CommPortNumber << "\n";
    }

    loadparams();
    ManualControlMenu();
    UnloadDLL();
    return 0;
}
