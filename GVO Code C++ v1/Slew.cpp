#include "Slew.h"

#include <cmath>
#include <iostream>
#include <windows.h>

#include "CommUtils.h"
#include "GetQandYU.h"
#include "GlobalValues.h"
#include "quad1to1.h"
#include "quad1to3.h"
#include "quad3to1.h"
#include "quad3to3.h"

void SlewScope(double raTarget, double decTarget) {
	double Alt = 0.0;
	double HA = 0.0;
	double Xcount = 0.0;
	double Ycount = 0.0;
	int localTargetQuadrant = 0;
	int localTargetYPole = 0;

	std::cout << "SlewScope: request received "
			  << "current quadrant=" << quadrant
			  << " current yPole=" << yPole
			  << " target RA=" << raTarget
			  << " target Dec=" << decTarget
			  << "\n";

	GetQandY(
		raTarget,
		decTarget,
		Alt,
		HA,
		Xcount,
		Ycount,
		localTargetQuadrant,
		localTargetYPole
	);

	std::cout << "SlewScope: target solution "
			  << "targetQuadrant=" << localTargetQuadrant
			  << " targetYPole=" << localTargetYPole
			  << " Alt=" << Alt
			  << " HA=" << HA
			  << " Xcount=" << Xcount
			  << " Ycount=" << Ycount
			  << "\n";

	if (Alt < 20.0 || localTargetYPole == 0) {
		if (Coord* coord = CommUtils::GetCoordPtr()) {
			coord->RAGoto = 0.0;
			coord->DecGoto = 0.0;
		}
		std::cout << "SlewScope: No slew, target below horizon or invalid\n";
		return;
	}

	bool slewStarted = false;

	if (localTargetQuadrant == 1 && quadrant == 1) {
		std::cout << "SlewScope: executing GoQuad1to1\n";
		slewStarted = GoQuad1to1(Xcount, Ycount);
	}
	else if (localTargetQuadrant == 3 && quadrant == 1) {
		std::cout << "SlewScope: executing GoQuad1to3\n";
		slewStarted = GoQuad1to3(Xcount, Ycount, decTarget);
	}
	else if (localTargetQuadrant == 3 && quadrant == 3) {
		std::cout << "SlewScope: executing GoQuad1to3\n";
		slewStarted = GoQuad1to3(Xcount, Ycount, decTarget);
	}
	else if (localTargetQuadrant == 1 && quadrant == 3) {
		std::cout << "SlewScope: executing GoQuad3to1\n";
		slewStarted = GoQuad3to1(Xcount, Ycount, decTarget);
	}
	else {
		if (Coord* coord = CommUtils::GetCoordPtr()) {
			coord->RAGoto = 0.0;
			coord->DecGoto = 0.0;
		}
	}

	if (slewStarted) {
		// Pascal behavior: the quad routine marks moving true and returns.
		if (Coord* coord = CommUtils::GetCoordPtr()) {
			coord->RAGoto = 0.0;
			coord->DecGoto = 0.0;
		}

		std::cout << "SlewScope: slew started\n";
	}
	else {
		std::cout << "SlewScope: slew failed to send\n";
	}
}