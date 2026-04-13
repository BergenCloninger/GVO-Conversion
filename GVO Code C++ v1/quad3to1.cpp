#include "quad3to1.h"
#include "GlobalValues.h"
#include "CommUtils.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>

bool GoQuad3to1(double Xcount, double Ycount, double decTarget) {
	movingRA = true;
	movingDEC = true;

	RaPos = Xcount;

	std::ostringstream raStream;
	raStream << std::fixed << std::setprecision(0) << RaPos;
	std::string raSteps = raStream.str();

	std::string raCmd = "AX  VL" + xvlslew + " MA" + raSteps + " GD ID;";

	decPos = std::abs(Ycount);

	std::ostringstream decStream;
	decStream << std::fixed << std::setprecision(0) << decPos;
	std::string decSteps = decStream.str();

	std::string decCmd;
	if (decTarget < C_Lat) {
		decCmd = "AY  VL" + yvlslew + " MA-" + decSteps + " GD ID;";
	} else {
		decCmd = "AY  VL" + yvlslew + " MA" + decSteps + " GD ID;";
	}

	std::cout << "GoQuad3to1:\n";
	std::cout << "  Xcount=" << Xcount << " Ycount=" << Ycount << "\n";
	std::cout << "  decTarget=" << decTarget << " C_Lat=" << C_Lat << "\n";
	std::cout << "  RA cmd: [" << raCmd << "]\n";
	std::cout << "  DEC cmd: [" << decCmd << "]\n";

	std::cout << "[GoQuad3to1] before AX send\n";
	bool raOk = SendCommand(raCmd);
	std::cout << "[GoQuad3to1] after AX send, raOk=" << raOk << "\n";

	if (!raOk) {
		std::cout << "Failed to send RA slew command\n";
		return false;
	}

	std::cout << "[GoQuad3to1] before AY send\n";
	bool decOk = SendCommand(decCmd);
	std::cout << "[GoQuad3to1] after AY send, decOk=" << decOk << "\n";

	if (!decOk) {
		std::cout << "Failed to send DEC slew command\n";
		return false;
	}

	return true;
}