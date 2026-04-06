#include "TimerUnit.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cctype>
#include "CommUtils.h"
#include "OMS68SERMC.h"
#include "CalcCoord.h"
#include "Slew.h"
#include "Sync.h"
#include "HandPadle.h"
#include "GlobalValues.h"

extern char Response[256];
Coord* CoordMem = CommUtils::GetCoordPtr(); //from shared memory

void TimerUpdate() {
	std::cout << "TimerUpdate: start" << std::endl;

	if (!CoordMem) {
		std::cout << "CoordMem is null, is TheSky running?\n";
		return;
	}

	std::string CmdStr;
	std::string CmdStr2;
	std::string TempStr;

	// if (NoPassword) {
	// 	CmdStr = "AA ST;";
	// 	SendCommand(CmdStr);
	// 	return;
	// }

	if (movingRA) {
		SendAndGetCommand(&CommRecord, "AX QA;", Response, sizeof(Response));
		TempStr = Response;

		if (TempStr.length() == 4) {
			if (std::toupper(TempStr[1]) == 'D') {
				movingRA = false;

				std::cout << "RA Axis Move Complete\n";

				char buf[64];
				sprintf(buf, "%.6f", TrkRate);

				CmdStr2 = buf;
				CmdStr = "AX JF" + CmdStr2 + ";";

				SendCommand(CmdStr);

				CoordMem->RAGoto = 0.0;
			}
		}
	}
	std::cout << "TimerUpdate: step 1" << std::endl;

	if (movingDEC) {
		SendAndGetCommand(&CommRecord, "AY QA;", Response, sizeof(Response));

		TempStr = Response;

		if (TempStr.length() == 4)
		{
			if (std::toupper(TempStr[1]) == 'D')
			{
				movingDEC = false;

				std::cout << "DEC Axis Move Complete\n";

				CoordMem->DecGoto = 0.0;
			}
		}
	}
	std::cout << "TimerUpdate: step 2" << std::endl;


	HalfSecondCounter++;

	if (HalfSecondCounter >= 5)
		HalfSecondCounter = 0;

	if (HalfSecondCounter == 4) {
		UpdateCoord();
	}
	std::cout << "TimerUpdate: step 3" << std::endl;

	if (movingRA || movingDEC)
		return;
	if (CoordMem->RASync != 0.0 && CoordMem->DecSync != 0.0) {
		SyncScope();
	}
	std::cout << "TimerUpdate: step 4" << std::endl;


	if (CoordMem->RAGoto != 0.0 && CoordMem->DecGoto != 0.0) {
		SlewScope();
		return;
	}
	std::cout << "TimerUpdate: step 5" << std::endl;


	if (Parkit) {
		Parkit = false;

		movingRA = true;
		movingDEC = true;
		SendCommand("AX KL;");

		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		SendCommand("AA VL75000,50000; MA0,0,,; GD; ID;");

		std::cout << "Parking Telescope...\n";

		return;
	}
	std::cout << "TimerUpdate: step 6" << std::endl;

	HandleHandPadle();
}

void AddTicks() {
	std::cout << "Quadrant: " << quadrant << "\n";

	if (yPole > 0)
		std::cout << "Y Polarity: +\n";
	else
		std::cout << "Y Polarity: -\n";
}


void Padle_Timer_Update() {
	HandleHandPadle();
}
