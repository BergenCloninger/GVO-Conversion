#include "Sync.h"
#include "CommUtils.h"
#include "CalcCoord.h"
#include "OMS68SERMC.h"
#include "GlobalValues.h"
#include "GetQandYU.h"
#include "utils.h"
#include <windows.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

void SyncScope() {
	if (movingRA || movingDEC)
		return;

	Coord* coord = CommUtils::GetCoordPtr();
	if (!coord) {
		std::cerr << "SyncScope: Coord is null\n";
		return;
	}

	double Alt, HA, Xcount, Ycount;
	int quad;

	GetQandY(coord->RASync, coord->DecSync, Alt, HA, Xcount, Ycount, quad, yPole);

	if (Alt < 0.0) {
		std::cerr << "Sync below Horizon\n";
		return;
	}

	std::string CmdStr, PreStrng;

	// RA position load
	if (Xcount < 0.0)
		PreStrng = "AX LP-";
	else
		PreStrng = "AX LP";

	Xcount = std::abs(Xcount);

	std::ostringstream raStream;
	raStream << std::fixed << std::setprecision(0) << Xcount;
	CmdStr = PreStrng + raStream.str() + ";";
	SendCommand(CmdStr);

	// Back to tracking
	CmdStr = "AX JF" + FormatDouble(TrkRate, 10, 6) + ";";
	SendCommand(CmdStr);

	// DEC position load
	if (Ycount < 0.0)
		PreStrng = "AY LP-";
	else
		PreStrng = "AY LP";

	Ycount = std::abs(Ycount);

	std::ostringstream decStream;
	decStream << std::fixed << std::setprecision(0) << Ycount;
	CmdStr = PreStrng + decStream.str() + ";";
	SendCommand(CmdStr);

	coord->RASync = 0.0;
	coord->DecSync = 0.0;

	UpdateCoord();
}