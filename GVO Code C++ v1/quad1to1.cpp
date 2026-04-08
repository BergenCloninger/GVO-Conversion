#include "quad1to1.h"
#include "GlobalValues.h"
#include "CommUtils.h" // for SendCommand

#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>

void GoQuad1to1(double Xcount, double Ycount) {
	movingRA = true;
	movingDEC = true;

	RaPos = std::abs(Xcount);
	decPos = Ycount;

	std::ostringstream raStepStream;
	raStepStream << std::fixed << std::setprecision(0) << RaPos;
	std::string raSteps = raStepStream.str();

	std::string raCmd = "AX VL" + xvlslew + " MA-" + raSteps + " GD ID;";

	std::ostringstream decStepStream;
	decStepStream << std::fixed << std::setprecision(0) << std::abs(decPos);
	std::string decSteps = decStepStream.str();

	std::string decCmd;
	if (decPos < 0.0) {
		decCmd = "AY VL" + yvlslew + " MA-" + decSteps + " GD ID;";
	} else {
		decCmd = "AY VL" + yvlslew + " MA" + decSteps + " GD ID;";
	}

	std::cout << "GoQuad1to1:\n";
	std::cout << "  Xcount=" << Xcount << " Ycount=" << Ycount << "\n";
	std::cout << "  RA cmd: " << raCmd << "\n";
	std::cout << "  DEC cmd: " << decCmd << "\n";

    // if (!SendCommand("AX ST;")) { TODO: The pascal code has a stop command set but not called, unsure if it's a bug or intentional. If intentional, add it in during testing (for all x to x quad functions)
	// 	std::cout << "Failed to send AX ST;\n";
	// }

	if (!SendCommand(raCmd)) {
		std::cout << "Failed to send RA slew command\n";
	}

	if (!SendCommand(decCmd)) {
		std::cout << "Failed to send DEC slew command\n";
	}
}