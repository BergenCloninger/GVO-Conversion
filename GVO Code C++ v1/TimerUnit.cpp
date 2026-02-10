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


	HalfSecondCounter++;

	if (HalfSecondCounter >= 5)
		HalfSecondCounter = 0;

	if (HalfSecondCounter == 4) {
		UpdateCoord();
	}

	if (movingRA || movingDEC)
		return;
	if (CoordMem->RASync != 0.0 && CoordMem->DecSync != 0.0) {
		SyncScope();
	}


	if (CoordMem->RAGoto != 0.0 && CoordMem->DecGoto != 0.0) {
		SlewScope();
		return;
	}



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
