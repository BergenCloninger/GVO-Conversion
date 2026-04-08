#include "quad1to3.h"
#include "GlobalValues.h"
#include "CommUtils.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>

void GoQuad1to3(double Xcount, double Ycount) {
	movingRA = true;
	movingDEC = true;

	RaPos = Xcount;

	std::ostringstream raStream;
	raStream << std::fixed << std::setprecision(0) << RaPos;
	std::string raSteps = raStream.str();

	std::string raCmd = "AX VL" + xvlslew + " MA" + raSteps + " GD ID;";

	decPos = std::abs(Ycount);

	std::ostringstream decStream;
	decStream << std::fixed << std::setprecision(0) << decPos;
	std::string decSteps = decStream.str();

	std::string decCmd;
	if (DecTarget < C_Lat) {
		decCmd = "AY VL" + yvlslew + " MA-" + decSteps + " GD ID;";
	} else {
		decCmd = "AY VL" + yvlslew + " MA" + decSteps + " GD ID;";
	}

	std::cout << "GoQuad1to3:\n";
	std::cout << "  Xcount=" << Xcount << " Ycount=" << Ycount << "\n";
	std::cout << "  DecTarget=" << DecTarget << " C_Lat=" << C_Lat << "\n";
	std::cout << "  RA cmd: " << raCmd << "\n";
	std::cout << "  DEC cmd: " << decCmd << "\n";

	if (!SendCommand(raCmd)) {
		std::cout << "Failed to send RA slew command\n";
	}

	if (!SendCommand(decCmd)) {
		std::cout << "Failed to send DEC slew command\n";
	}
}