#include "GlobalValues.h"
#include "CommUtils.h"
#include "StateVar.h"
#include <thread>
#include <chrono>
#include <string>

// External helpers
extern std::string FormatDouble(double v, int w, int p);
extern void BumpNorth();
extern void BumpSouth();

void BumpNorth() {
	// Small south pulse first
	SendCommand("AY JG-50;");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	SendCommand("AY ST;");

	LastDecNorth = true;
}

void BumpSouth() {
	// Small north pulse first
	SendCommand("AY JG50;");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	SendCommand("AY ST;");

	LastDecNorth = false;
}

void HandleFastPadle() {
	std::string CmdStr;
	std::string TempStr;
	std::string xspeed, yspeed;

	// Select speed (5 inch guide vs slew)
	if (!SlewSelect) {
		xspeed = xvlslew;   // high rate
		yspeed = yvlslew;
	}
	else {
		xspeed = xvl5inch;  // 5 inch guide
		yspeed = yvl5inch;
	}

	switch (Xstate) {
	case StateVar::Off:
		break;


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
		if (!EastPushed && !WestPushed) {
			Xstate = StateVar::Tracking;

			SendCommand("AX ST;");

			CmdStr = "AX JF" + FormatDouble(TrkRate, 10, 6) + ";";
			SendCommand(CmdStr);
		}

		if (EastPushed && WestPushed) {
			// Stay in CorrectingE (as Pascal)
			Xstate = StateVar::CorrectingE;
		}

		break;


	case StateVar::CorrectingW:
		if (!EastPushed && !WestPushed) {
			Xstate = StateVar::Tracking;

			SendCommand("AX ST;");

			CmdStr = "AX JF" + FormatDouble(TrkRate, 10, 6) + ";";
			SendCommand(CmdStr);
		}

		if (EastPushed && WestPushed) {
			// Stay in CorrectingW
			Xstate = StateVar::CorrectingW;
		}

		break;


	case StateVar::Slewing:
		if (!EastPushed && !WestPushed) {
			Xstate = StateVar::Tracking;

			SendCommand("AX ST;");

			CmdStr = "AX JF" + FormatDouble(TrkRate, 10, 6) + ";";
			SendCommand(CmdStr);
		}

		break;
	}

	switch (Ystate) {
	case StateVar::Off:
		break;

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
		if (!NorthPushed && !SouthPushed) {
			Ystate = StateVar::Tracking;

			SendCommand("AY ST;");
		}

		if (NorthPushed && SouthPushed) {
			Ystate = StateVar::Slewing;
		}

		break;


	case StateVar::CorrectingS:
		if (!NorthPushed && !SouthPushed) {
			Ystate = StateVar::Tracking;

			SendCommand("AY ST;");
		}

		if (NorthPushed && SouthPushed) {
			Ystate = StateVar::Slewing;
		}

		break;


	case StateVar::Slewing:
		if (!NorthPushed && !SouthPushed) {
			Ystate = StateVar::Tracking;

			SendCommand("AY ST;");
		}

		break;
	}
}
