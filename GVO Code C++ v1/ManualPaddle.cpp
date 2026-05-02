#include "GlobalValues.h"
#include "CommUtils.h"
#include "StateVar.h"

#include <thread>
#include <chrono>
#include <string>
#include <iostream>

extern std::string FormatDouble(double v, int w, int p);

void BumpNorth() {
	SendCommand("AY JG-" + std::to_string(DecBumpRate) + ";");
	std::this_thread::sleep_for(std::chrono::milliseconds(DecBumpDurationMs));
	SendCommand("AY ST;");
	LastDecNorth = true;
}

void BumpSouth() {
	SendCommand("AY JG" + std::to_string(DecBumpRate) + ";");
	std::this_thread::sleep_for(std::chrono::milliseconds(DecBumpDurationMs));
	SendCommand("AY ST;");
	LastDecNorth = false;
}

void HandleFastPaddle() {
	std::string CmdStr;
	std::string xspeed, yspeed;

	if (!SlewSelect) {
		xspeed = FastPadXSlew;
		yspeed = FastPadYSlew;
	} else {
		xspeed = FastPadXFine;
		yspeed = FastPadYFine;
	}

	switch (Xstate) {
	case StateVar::Tracking:
		if (EastPushed) {
			Xstate = StateVar::CorrectingE;

			CmdStr = "AX ST; AX JG-" + xspeed + ";";
			SendCommand(CmdStr);
		}

		if (WestPushed) {
			Xstate = StateVar::CorrectingW;

			CmdStr = "AX ST; AX JG" + xspeed + ";";
			SendCommand(CmdStr);
		}
		break;

	case StateVar::CorrectingE:
	case StateVar::CorrectingW:
		if (!EastPushed && !WestPushed) {
			Xstate = StateVar::Tracking;

			SendCommand("AX ST;");
			CmdStr = "AX JF" + FormatDouble(TrkRate, 10, 6) + ";";
			SendCommand(CmdStr);
		}
		break;

	default:
		break;
	}

	switch (Ystate) {
	case StateVar::Tracking:
		if (NorthPushed) {
			if (yPole >= 0 && LastDecNorth)
				BumpSouth();
			else if (!LastDecNorth)
				BumpNorth();

			Ystate = StateVar::CorrectingN;

			CmdStr = "AY JG" + yspeed + ";";
			SendCommand(CmdStr);
		}

		if (SouthPushed) {
			if (yPole >= 0 && !LastDecNorth)
				BumpNorth();
			else if (LastDecNorth)
				BumpSouth();

			Ystate = StateVar::CorrectingS;

			CmdStr = "AY JG-" + yspeed + ";";
			SendCommand(CmdStr);
		}
		break;

	case StateVar::CorrectingN:
	case StateVar::CorrectingS:
		if (!NorthPushed && !SouthPushed) {
			Ystate = StateVar::Tracking;
			SendCommand("AY ST;");
		}
		break;

	default:
		break;
	}
}