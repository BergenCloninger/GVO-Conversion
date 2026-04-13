#include "quad1to1.h"
#include "GlobalValues.h"
#include "CommUtils.h" // for SendCommand

#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>

bool GoQuad1to1(double Xcount, double Ycount) {
	movingRA = true;
	movingDEC = true;

	RaPos = std::abs(Xcount);
	decPos = Ycount;

	std::ostringstream raStepStream;
	raStepStream << std::fixed << std::setprecision(0) << RaPos;
	std::string raSteps = raStepStream.str();

	std::string raCmd = "AX  VL" + xvlslew + " MA-" + raSteps + " GD ID;";

	std::ostringstream decStepStream;
	decStepStream << std::fixed << std::setprecision(0) << std::abs(decPos);
	std::string decSteps = decStepStream.str();

	std::string decCmd;
	if (decPos < 0.0) {
		decCmd = "AY  VL" + yvlslew + " MA-" + decSteps + " GD ID;";
	} else {
		decCmd = "AY  VL" + yvlslew + " MA" + decSteps + " GD ID;";
	}

	bool raOk = SendCommand(raCmd);
	if (!raOk) {
		movingRA = false;
		movingDEC = false;
		return false;
	}

	bool decOk = SendCommand(decCmd);
	if (!decOk) {
		movingRA = false;
		movingDEC = false;
		return false;
	}

	return true;
}