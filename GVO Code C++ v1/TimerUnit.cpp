#include "TimerUnit.h"

#include <cctype>
#include <cstdio>
#include <iostream>

#include "CalcCoord.h"
#include "CommUtils.h"
#include "GlobalValues.h"
#include "HandPadle.h"
#include "OMS68SERMC.h"
#include "Slew.h"
#include "Sync.h"

extern char Response[256];

void TimerUpdate() {
	static bool inTimerUpdate = false;
	if (inTimerUpdate) {
		return;
	}
	inTimerUpdate = true;

	struct TimerGuard {
		bool& flag;
		~TimerGuard() { flag = false; }
	} guard{inTimerUpdate};

	Coord* CoordMem = CommUtils::GetCoordPtr();
	if (!CoordMem) {
		std::cout << "CoordMem is null\n";
		return;
	}

	// -------------------- If moving, check for end of move --------------------
	if (movingRA) {
		char axResp[256] = {};
		if (!SendAndGetCommand(&CommRecord, "AX QA;", axResp, sizeof(axResp))) {
			return;
		}

		std::string tempStr = axResp;

		if (tempStr.length() != 4) {
			return;
		}

		if (std::toupper(static_cast<unsigned char>(tempStr[1])) == 'D') {
			movingRA = false;
			std::cout << "RA Axis Move Complete\n";

			char buf[64];
			std::sprintf(buf, "%.6f", TrkRate);

			std::string cmdStr = "AX JF" + std::string(buf) + ";";
			std::cout << "[TimerUpdate] SEND: [" << cmdStr << "]\n";
			SendCommand(cmdStr);

			Xstate = StateVar::Tracking;
			Ystate = StateVar::Tracking;
			std::cout << "[TimerUpdate] Xstate/Ystate set to Tracking after move complete\n";

			CoordMem->RAGoto = 0.0;
		}
	}

	if (movingDEC) {
		char ayResp[256] = {};
		if (!SendAndGetCommand(&CommRecord, "AY QA;", ayResp, sizeof(ayResp))) {
			return;
		}

		std::string tempStr = ayResp;

		if (tempStr.length() != 4) {
			return;
		}

		if (std::toupper(static_cast<unsigned char>(tempStr[1])) == 'D') {
			movingDEC = false;
			std::cout << "DEC Axis Move Complete\n";

			CoordMem->DecGoto = 0.0;
		}
	}

	HalfSecondCounter++;
	if (HalfSecondCounter >= 5) {
		HalfSecondCounter = 0;
	}

	if (HalfSecondCounter == 4) {
		UpdateCoord();
	}

	if (movingRA || movingDEC) {
		return;
	}

	if (CoordMem->RASync != 0.0 && CoordMem->DecSync != 0.0) {
		SyncScope();
		return;
	}

	if (CoordMem->RAGoto != 0.0 && CoordMem->DecGoto != 0.0) {
		double raTarget = CoordMem->RAGoto;
		double decTarget = CoordMem->DecGoto;

		std::cout << "[TimerUpdate] starting slew request "
				  << "RAGoto=" << raTarget
				  << " DecGoto=" << decTarget
				  << "\n";

		SlewScope(raTarget, decTarget);
		return;
	}

	if (Parkit) {
		Parkit = false;

		movingRA = true;
		movingDEC = true;

		SendCommand("AX KL;");
		Sleep(300);
		SendCommand("AA VL75000,50000; MA0,0,,; GD; ID;");

		return;
	}

	if (parkInProgress && !movingRA && !movingDEC) {
		char buf[64];
		sprintf(buf, "%.6f", TrkRate);

		std::string cmd = "AX JF" + std::string(buf) + ";";
		std::cout << "[PARK COMPLETE] " << cmd << "\n";
		SendCommand(cmd);

		Xstate = StateVar::Tracking;
		Ystate = StateVar::Tracking;
		std::cout << "[PARK COMPLETE] Xstate/Ystate set to Tracking\n";
				
		parkInProgress = false;
		parkCompleted = true;
	}

	HandleHandPadle();
}

void PrintCoordState(Coord* c) {
	static Coord last = {};
	static bool first = true;

	if (first ||
		c->RA != last.RA ||
		c->Dec != last.Dec ||
		c->RAGoto != last.RAGoto ||
		c->DecGoto != last.DecGoto ||
		c->RASync != last.RASync ||
		c->DecSync != last.DecSync) {

		std::cout
			<< "RA=" << c->RA
			<< " Dec=" << c->Dec
			<< " RAGoto=" << c->RAGoto
			<< " DecGoto=" << c->DecGoto
			<< " RASync=" << c->RASync
			<< " DecSync=" << c->DecSync
			<< std::endl;

		last = *c;
		first = false;
	}
}

void AddTicks() {
	std::cout << "Quadrant: " << quadrant << "\n";

	if (yPole > 0) {
		std::cout << "Y Polarity: +\n";
	}
	else {
		std::cout << "Y Polarity: -\n";
	}
}

void Padle_Timer_Update() {
	HandleHandPadle();
}