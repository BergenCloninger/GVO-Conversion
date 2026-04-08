#include "quad2to2.h"
#include "GlobalValues.h"
#include "CommUtils.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>

void GoQuad2to2() {
	movingRA = true;
	movingDEC = true;

	// RA calculation
	RaPos = RaTarget - EastHor;
	if (RaPos > 0.0)
		RaPos = 24.0 - RaPos;

	RaPos = std::abs(RaPos);
	RaPos = (RaPos * RAFact) * 15.0;

	std::ostringstream raStream;
	raStream << std::fixed << std::setprecision(0) << RaPos;
	std::string raSteps = raStream.str();

	std::string raCmd = "AX VL" + xvlslew + " MA" + raSteps + " GD ID;";

	// DEC calculation
	decPos = 90.0 - DecTarget;
	if (DecTarget < 0.0)
		decPos = 90.0 + std::abs(DecTarget);

	decPos = decPos * DECFACT;

	std::ostringstream decStream;
	decStream << std::fixed << std::setprecision(0) << decPos;
	std::string decSteps = decStream.str();

	std::string decCmd = "AY VL" + yvlslew + " MA" + decSteps + " GD ID;";

	std::cout << "GoQuad2to2:\n";
	std::cout << "  RaTarget=" << RaTarget << " DecTarget=" << DecTarget << "\n";
	std::cout << "  EastHor=" << EastHor << "\n";
	std::cout << "  RA cmd: " << raCmd << "\n";
	std::cout << "  DEC cmd: " << decCmd << "\n";

	if (!SendCommand(raCmd)) {
		std::cout << "Failed to send RA slew command\n";
	}

	if (!SendCommand(decCmd)) {
		std::cout << "Failed to send DEC slew command\n";
	}
}