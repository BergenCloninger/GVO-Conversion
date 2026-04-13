#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <cmath>
#include <cstdint>
#include <functional>
#include <windows.h>
#include <conio.h>
#include "OMS68SERMC.h"
#include "CommUtils.h"
#include "TimerUnit.h"
#include "GlobalValues.h"
#include "StateVar.h"
#include "Config.h"
#include "utils.h"

double TrkRate = 0.0;
double RAFact = 0.0, DECFACT = 0.0;
double C_Lat = 0.0, C_Long = 0.0;

double SidTime = 0.0;
double RANow = 0.0, DECNow = 0.0;
double RaTarget = 0.0, DecTarget = 0.0, Meridian = 0.0, EastHor = 0.0, WestHor = 0.0;
double EastHA = 0.0, WestHA = 0.0, NorthHA = 0.0, SouthHA = 0.0, A = 0.0, H = 0.0;
double RaPos = 0.0, decPos = 0.0, tdecfact = 0.0;

std::string xvlslew, yvlslew, xvl5inch, yvl5inch;
std::string xvl, xac, xacmax, xvlmax, yvl, yac, yacmax, yvlmax;

int DecBack = 0;
int yPole = 1;
bool LastDecNorth = true;
bool SlewSelect = true;

bool EastOfMeridian = true;
bool LookingEast = true;
bool TargetEastOfMeridian = true;
bool TimerEnabled = true;

uint8_t keystroke = 0, pcxdataout = 0, status = 0, HalfSecondCounter = 0;
int quadrant = 1, targetQuadrant = 1;
bool movingRA = false, movingDEC = false;
uint8_t io = 0;
bool EastPushed = false, WestPushed = false, NorthPushed = false, SouthPushed = false, ManualPushed = false;

int RAHr = 0, RAMin = 0, RASec = 0;
int DECDeg = 0, DECMin = 0, DECSec = 0;
int altdeg = 0, altmin = 0, altsec = 0, azdeg = 0, azmin = 0, azsec = 0;

bool Parkit = false, NorthofZenith = false, NoPassword = false;
char Response[256] = {0};

//MATHEMATICAL CONSTANTS SET
double pi = 3.1415926535897932;
double pi2 = 2.0 * pi;

double DToR = pi / 180.0;
double HToR = pi / 12.0;
double RToD = 180.0 / pi;
double RToH = 12.0 / pi;
double SToR = DToR / 3600.0;

double EarthRadius = 6378.14;
double SolarParallax = 8.794 * SToR;

int FK4System = 1;
int FK5System = 2;

//Com port and DLL paths (defaults, override in config.ini)
int ComPortNumber = 1;
std::string OmsDllPath = "68SERMC.DLL";
std::string TeleApiDllPath = "teleapi.dll";

std::string name, commandBuffer;
using TeleAPICallBack = std::function<void(int)>;

StateVar Xstate = StateVar::Off;
StateVar Ystate = StateVar::Off;

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

void loadparams() {
	tdecfact = ((360.0 / 338.0) * 3600.0) / 50000.0;
	tdecfact = 1.0 / tdecfact;
	tdecfact *= 3600.0;

	TrkRate = 500.6;
	RAFact = 120000.0;
	DECFACT = tdecfact;
	DecBack = 30;
	C_Lat = 38.508741;
	C_Long = 106.9385583;

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

	yPole = 1;
	NoPassword = true;
}

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
                while (true) {
                    if (TimerEnabled) {
                        TimerUpdate();
                    }

                    if (_kbhit()) {
                        int ch = _getch();
                        if (ch == 27) {
                            std::cout << "Returning to menu...\n";
                            break;
                        }
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
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
    loadparams();
    SetStime();
    LoadConfig("config.ini");
	ApplyConfig();
    PrintConfig();

    //DLL init
    if (!CommUtils::InitSharedMem(TeleApiDllPath)) {
        std::cout << "Failed to init shared memory\n";
        return 1;
    }

    if (!LoadDLL(OmsDllPath)) return 1;
    CommRecord.CommPortNumber = ComPortNumber;

    if (!InitComm()) {
        std::cerr << "Failed to initialize telescope." << std::endl;
        return 1;
    } else {
        std::cout << "Connected to device on port " << CommRecord.CommPortNumber << "\n";
    }

	char buf[64];
	sprintf(buf, "%.6f", TrkRate);

	std::string cmd = "AX JF" + std::string(buf) + ";";
	std::cout << "[INIT TRACKING] " << cmd << "\n";

	if (!SendCommand(cmd)) {
		std::cout << "Failed to start RA tracking!\n";
	}

    ManualControlMenu();
    UnloadDLL();
    return 0;
}
