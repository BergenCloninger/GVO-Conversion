#include "TimerUnit.h"
#include "OMS68SERMC.h"
#include "CommUtils.h"
#include <iostream>
#include <string>
#include <cctype>
#include <cmath>
#include <windows.h>

using namespace CommUtils; // namespace for shared memory and functions

// External globals from main.cpp
extern bool NoPassword;
extern bool movingRA, movingDEC;
extern bool Parkit;
extern bool SlewSelect;
extern bool LookingEast;
extern bool EastOfMeridian;
extern bool TargetEastOfMeridian;
extern bool LastDecNorth;

extern int Quadrant;
extern int halfSecondCounter;
extern int yPole;

extern double TrkRate;

extern std::string xvl, yvl;

extern char Response[256];

void SyncScope() {
	Coord* C = GetCoordPtr();
	if (!C || movingRA || movingDEC)
		return;

	double Alt = 0.0, HA = 0.0, Xcount = 0.0, Ycount = 0.0;
	int Quad = 0, YpoleLocal = 0;

	// TODO: implement GetQandY to populate Alt, HA, Xcount, Ycount, Quad, YpoleLocal
	// GetQandY(C->RASync, C->DecSync, Alt, HA, Xcount, Ycount, Quad, YpoleLocal);

	if (Alt < 0.0) {
		std::cerr << "Sync below Horizon" << std::endl;
		exit(1);
	}

	std::string CmdStr;

	// RA Position
	CmdStr = (Xcount < 0 ? "AX LP-" : "AX LP") + std::to_string(static_cast<long>(std::abs(Xcount))) + ";";
	SendCommand(CmdStr);

	// RA Tracking
	CmdStr = "AX JF" + std::to_string(TrkRate) + ";";
	SendCommand(CmdStr);

	// DEC Position
	CmdStr = (Ycount < 0 ? "AY LP-" : "AY LP") + std::to_string(static_cast<long>(std::abs(Ycount))) + ";";
	SendCommand(CmdStr);

	// Reset sync request
	C->RASync = 0.0;
	C->DecSync = 0.0;
}


void Padle_Timer_Update() {
	// HandleHandPadle();
}

void TimerUpdate() {
	std::string CmdStr, TempStr;

	if (NoPassword) {
		SendCommand("AA ST;");
		return;
	}

	// RA axis movement
	if (movingRA) {
		if (SendAndGetCommand(&CommRecord, "AX QA;", Response, sizeof(Response))) {
			TempStr = Response;
			if (TempStr.length() == 4 && toupper(TempStr[1]) == 'D') {
				movingRA = false;
				std::cout << "RA Axis Move Complete\n";

				char buf[64];
				snprintf(buf, sizeof(buf), "%10.6f", TrkRate);
				CmdStr = "AX JF" + std::string(buf) + ";";
				SendCommand(CmdStr);
			}
		}
	}

	// DEC axis movement
	if (movingDEC) {
		if (SendAndGetCommand(&CommRecord, "AY QA;", Response, sizeof(Response))) {
			TempStr = Response;
			if (TempStr.length() == 4 && toupper(TempStr[1]) == 'D') {
				movingDEC = false;
				std::cout << "DEC Axis Move Complete\n";
			}
		}
	}

	halfSecondCounter++;
	if (halfSecondCounter >= 5) halfSecondCounter = 0;

	if (halfSecondCounter == 4) {
		// UpdateCoord();
	}

	if (movingRA || movingDEC)
		return;

	Coord* C = GetCoordPtr();
	if (C && C->RASync != 0.0 && C->DecSync != 0.0) {
		SyncScope();
	}

	if (C && C->RAGoto != 0.0 && C->DecGoto != 0.0) {
		// SlewScope();
		return;
	}

	// Parking sequence
	if (Parkit) {
		Parkit = false;
		movingRA = true;
		movingDEC = true;

		SendCommand("AX KL;");
		Sleep(300);
		SendCommand("AA VL75000,50000; MA0,0,,; GD; ID;");

		std::cout << "Parking sequence executed.\n";
		return;
	}

	// HandleHandPadle();
}


// void AddTicks() {
// 	if (yPole > 0)
// 		std::cout << "YPole Polarity: +\n";
// 	else
// 		std::cout << "YPole Polarity: -\n";

// 	switch (Quadrant) {
// 	case 1: std::cout << "[Quadrant 1]\n"; break;
// 	case 2: std::cout << "[Quadrant 2]\n"; break;
// 	case 3: std::cout << "[Quadrant 3]\n"; break;
// 	case 4: std::cout << "[Quadrant 4]\n"; break;
// 	}
// }
