#include "HandPadle.h"
#include "CommUtils.h"
#include "OMS68SERMC.h"
#include "CalcCoord.h"
#include "TimerUnit.h"
#include "StateVar.h"
#include "utils.h"
#include "GlobalValues.h"
#include "ManualPadle.h"
#include "Bits.h"
#include <string>
#include <cmath>

void HandleHandPadle()
{
	std::string CmdStr, TempStr;
	int i;

	// Reset button states
	EastPushed = WestPushed = NorthPushed = SouthPushed = false;

	// Read handpad
	SendAndGetCommand(&CommRecord, "BX;", Response, sizeof(Response));
	TempStr = Response;

	if (TempStr.length() != 2)
		return;

	// ---------- Decode first nibble ----------
	uint8_t ioLocal = TempStr[0];

	ioLocal = (ioLocal > '9') ? ioLocal - 0x37 : ioLocal - 0x30;

	ManualPushed = (ioLocal & ManualBit) != 0;
	SouthPushed  = (ioLocal & SouthBit)  != 0;


	// ---------- Decode second nibble ----------
	ioLocal = TempStr[1];

	ioLocal = (ioLocal > '9') ? ioLocal - 0x37 : ioLocal - 0x30;

	NorthPushed = (ioLocal & NorthBit) != 0;
	EastPushed  = (ioLocal & EastBit)  != 0;
	WestPushed  = (ioLocal & WestBit)  != 0;


	// ---------- High speed mode ----------
	if (ManualPushed || SlewSelect)
	{
		HandleFastPadle();
		return;
	}


	// ================= X AXIS =================
	switch (Xstate)
	{
	case StateVar::Off:
		break;


	case StateVar::Tracking:

		if (EastPushed)
		{
			Xstate = StateVar::CorrectingE;

			i = (int)std::round(TrkRate / 2.0);

			CmdStr = "AX JG" + std::to_string(i) + ";";
			SendCommand(CmdStr);
		}

		if (WestPushed)
		{
			Xstate = StateVar::CorrectingW;

			i = (int)std::round(TrkRate + TrkRate / 2.0);

			CmdStr = "AX JG" + std::to_string(i) + ";";
			SendCommand(CmdStr);
		}
		break;


	case StateVar::CorrectingE:

		if (!EastPushed && !WestPushed)
		{
			Xstate = StateVar::Tracking;

			SendCommand("AX ST;");
			SendCommand("AX ST;");

			CmdStr = "AX JF" + FormatDouble(TrkRate, 10, 6) + ";";
			SendCommand(CmdStr);
		}

		if (EastPushed && WestPushed)
		{
			Xstate = StateVar::Slewing;

			CmdStr = "AX ST; AX JG-" + xvl + ";";
			SendCommand(CmdStr);
		}
		break;


	case StateVar::CorrectingW:

		if (!EastPushed && !WestPushed)
		{
			Xstate = StateVar::Tracking;

			SendCommand("AX ST;");
			SendCommand("AX ST;");

			CmdStr = "AX JF" + FormatDouble(TrkRate, 10, 6) + ";";
			SendCommand(CmdStr);
		}

		if (EastPushed && WestPushed)
		{
			Xstate = StateVar::Slewing;

			CmdStr = "AX JG" + xvl + ";";
			SendCommand(CmdStr);
		}
		break;


	case StateVar::Slewing:

		if (!EastPushed && !WestPushed)
		{
			Xstate = StateVar::Tracking;

			SendCommand("AX ST;");

			CmdStr = "AX JF" + FormatDouble(TrkRate, 10, 6) + ";";
			SendCommand(CmdStr);
		}
		break;


	default:
		break;
	}


	// ================= Y AXIS =================
	switch (Ystate)
	{
	case StateVar::Off:
		break;


	case StateVar::Tracking:

		if (NorthPushed)
		{
			if ((yPole >= 0 && LastDecNorth))
				BumpSouth();
			else if (!LastDecNorth)
				BumpNorth();

			Ystate = StateVar::CorrectingN;

			i = (int)std::round(TrkRate / 4.0);

			CmdStr = "AY JG" + std::to_string(i) + ";";
			SendCommand(CmdStr);
		}


		if (SouthPushed)
		{
			if ((yPole >= 0 && !LastDecNorth))
				BumpNorth();
			else if (LastDecNorth)
				BumpSouth();

			Ystate = StateVar::CorrectingS;

			i = (int)std::round(TrkRate / 4.0);

			CmdStr = "AY JG-" + std::to_string(i) + ";";
			SendCommand(CmdStr);
		}

		break;


	case StateVar::CorrectingN:

		if (!NorthPushed && !SouthPushed)
		{
			Ystate = StateVar::Tracking;

			SendCommand("AY ST;");
		}

		if (NorthPushed && SouthPushed)
		{
			Ystate = StateVar::Slewing;

			CmdStr = "AY JG" + yvl + ";";
			SendCommand(CmdStr);
		}
		break;


	case StateVar::CorrectingS:

		if (!NorthPushed && !SouthPushed)
		{
			Ystate = StateVar::Tracking;

			SendCommand("AY ST;");
		}

		if (NorthPushed && SouthPushed)
		{
			Ystate = StateVar::Slewing;

			CmdStr = "AY JG-" + yvl + ";";
			SendCommand(CmdStr);
		}
		break;


	case StateVar::Slewing:

		if (!NorthPushed && !SouthPushed)
		{
			Ystate = StateVar::Tracking;

			SendCommand("AY ST;");
		}
		break;


	default:
		break;
	}
}
