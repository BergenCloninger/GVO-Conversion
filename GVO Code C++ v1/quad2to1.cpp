#include "quad2to1.h"
#include "GlobalValues.h"
#include "CommUtils.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>

bool GoQuad2to1(double raTarget, double decTarget, double eastHor) {
	movingRA = true;
	movingDEC = true;

	RaPos = raTarget - eastHor;
	if (RaPos > 0.0)
		RaPos = 24.0 - RaPos;

	RaPos = std::abs(RaPos);
	RaPos = (RaPos * RAFact) * 15.0;

	std::ostringstream raStream;
	raStream << std::fixed << std::setprecision(0) << RaPos;
	std::string raSteps = raStream.str();

	std::string raCmd = "AX  VL" + xvlslew + " MA" + raSteps + " GD ID;";

	decPos = 90.0 - decTarget;
	if (decTarget < 0.0)
		decPos = 90.0 + std::abs(decTarget);

	decPos = decPos * DECFACT;

	std::ostringstream decStream;
	decStream << std::fixed << std::setprecision(0) << decPos;
	std::string decSteps = decStream.str();

	std::string decCmd = "AY  VL" + yvlslew + " MA" + decSteps + " GD ID;";

	std::cout << "GoQuad2to1:\n";
	std::cout << "  raTarget=" << raTarget << " decTarget=" << decTarget << "\n";
	std::cout << "  eastHor=" << eastHor << "\n";
	std::cout << "  RA cmd: [" << raCmd << "]\n";
	std::cout << "  DEC cmd: [" << decCmd << "]\n";

	std::cout << "[GoQuad2to1] before AX send\n";
	bool raOk = SendCommand(raCmd);
	std::cout << "[GoQuad2to1] after AX send, raOk=" << raOk << "\n";

	if (!raOk) {
		std::cout << "Failed to send RA slew command\n";
		return false;
	}

	std::cout << "[GoQuad2to1] before AY send\n";
	bool decOk = SendCommand(decCmd);
	std::cout << "[GoQuad2to1] after AY send, decOk=" << decOk << "\n";

	if (!decOk) {
		std::cout << "Failed to send DEC slew command\n";
		return false;
	}

	return true;
}