#include "HandPaddle.h"
#include "CommUtils.h"
#include "OMS68SERMC.h"
#include "CalcCoord.h"
#include "TimerUnit.h"
#include "StateVar.h"
#include "utils.h"
#include "GlobalValues.h"
#include "ManualPaddle.h"
#include "Bits.h"

#include <string>
#include <cmath>
#include <iostream>
#include <cctype>
#include <cstdint>

void HandleHandPaddle() {
	if (movingRA || movingDEC) {
		return;
	}

	std::string CmdStr, TempStr;
	int i;

	EastPushed = false;
	WestPushed = false;
	NorthPushed = false;
	SouthPushed = false;
	ManualPushed = false;

	SendAndGetCommand(&CommRecord, "BX;", Response, sizeof(Response));
	TempStr = Response;

	if (TempStr.length() != 2) {
		return;
	}

	uint8_t ioLocal = std::toupper(static_cast<unsigned char>(TempStr[0]));
	ioLocal = (ioLocal > '9') ? ioLocal - 0x37 : ioLocal - 0x30;

	ManualPushed = (ioLocal & ManualBit) != 0;
	SouthPushed  = (ioLocal & SouthBit)  != 0;

	ioLocal = std::toupper(static_cast<unsigned char>(TempStr[1]));
	ioLocal = (ioLocal > '9') ? ioLocal - 0x37 : ioLocal - 0x30;

	NorthPushed = (ioLocal & NorthBit) != 0;
	EastPushed  = (ioLocal & EastBit)  != 0;
	WestPushed  = (ioLocal & WestBit)  != 0;

	if (ManualPushed) {
		HandleFastPaddle();
		return;
	}

	switch (Xstate) {
		case StateVar::Tracking:
			if (EastPushed) {
				Xstate = StateVar::CorrectingE;
				i = static_cast<int>(std::round(TrkRate - HandPadRaEastRate));
				CmdStr = "AX JG" + std::to_string(i) + ";";
				SendCommand(CmdStr);
			}

			if (WestPushed) {
				Xstate = StateVar::CorrectingW;
				i = static_cast<int>(std::round(TrkRate + HandPadRaWestRate));
				CmdStr = "AX JG" + std::to_string(i) + ";";
				SendCommand(CmdStr);
			}
			break;

		case StateVar::CorrectingE:
			if (!EastPushed && !WestPushed) {
				Xstate = StateVar::Tracking;

				SendCommand("AX ST;");
				SendCommand("AX ST;");

				CmdStr = "AX JF" + FormatDouble(TrkRate, 10, 6) + ";";
				SendCommand(CmdStr);
			}

			if (EastPushed && WestPushed) {
				Xstate = StateVar::Slewing;
				CmdStr = "AX ST; AX JG-" + xvl + ";";
				SendCommand(CmdStr);
			}
			break;

		case StateVar::CorrectingW:
			if (!EastPushed && !WestPushed) {
				Xstate = StateVar::Tracking;

				SendCommand("AX ST;");
				SendCommand("AX ST;");

				CmdStr = "AX JF" + FormatDouble(TrkRate, 10, 6) + ";";
				SendCommand(CmdStr);
			}

			if (EastPushed && WestPushed) {
				Xstate = StateVar::Slewing;
				CmdStr = "AX JG" + xvl + ";";
				SendCommand(CmdStr);
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

		default:
			break;
	}

	switch (Ystate) {
		case StateVar::Tracking:
			if (NorthPushed) {
				if ((yPole >= 0 && LastDecNorth))
					BumpSouth();
				else if (!LastDecNorth)
					BumpNorth();

				Ystate = StateVar::CorrectingN;
				i = static_cast<int>(std::round(HandPadDecRate));
				CmdStr = "AY JG" + std::to_string(i) + ";";
				SendCommand(CmdStr);
			}

			if (SouthPushed) {
				if ((yPole >= 0 && !LastDecNorth))
					BumpNorth();
				else if (LastDecNorth)
					BumpSouth();

				Ystate = StateVar::CorrectingS;
				i = static_cast<int>(std::round(HandPadDecRate));
				CmdStr = "AY JG-" + std::to_string(i) + ";";
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