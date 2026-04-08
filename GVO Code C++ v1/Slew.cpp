#include "Slew.h"
#include "CommUtils.h"
#include "GetQandYU.h"
#include "GlobalValues.h"
#include "quad1to1.h"
#include "quad1to3.h"
#include "quad3to3.h"
#include "quad3to1.h"
#include <windows.h>
#include <cmath>
#include <iostream>

void SlewScope() {
	Coord* coord = CommUtils::GetCoordPtr();
	if (!coord) {
		std::cout << "SlewScope: Coord is null\n";
		return;
	}

	RaTarget = coord->RAGoto;
	DecTarget = coord->DecGoto;

	double Alt, HA, Xcount, Ycount;
	int i;

	GetQandY(RaTarget, DecTarget, Alt, HA, Xcount, Ycount, targetQuadrant, i);

	if (Alt < 30.0 || i == 0) {
		coord->RAGoto = 0.0;
		coord->DecGoto = 0.0;
		std::cout << "No slew: target below 30 degrees altitude or invalid\n";
		return;
	}

	bool slew_started = false;

	if (targetQuadrant == 1 && quadrant == 1) {
		GoQuad1to1(Xcount, Ycount);
		slew_started = true;
	}
	else if (targetQuadrant == 3 && quadrant == 1) {
		GoQuad1to3(Xcount, Ycount);
		slew_started = true;
	}
	else if (targetQuadrant == 3 && quadrant == 3) {
		GoQuad3to3(Xcount, Ycount);
		slew_started = true;
	}
	else if (targetQuadrant == 1 && quadrant == 3) {
		GoQuad3to1(Xcount, Ycount);
		slew_started = true;
	}
	else {
		coord->RAGoto = 0.0;
		coord->DecGoto = 0.0;
		std::cout << "No slew: unsupported quadrant transition "
		          << quadrant << " -> " << targetQuadrant << "\n";
		return;
	}

	if (slew_started) {
		movingRA = true;
		movingDEC = true;

		// Optional: consume the request now so TimerUpdate doesn't retrigger it
		// coord->RAGoto = 0.0;
		// coord->DecGoto = 0.0;

		std::cout << "Slew started: quadrant " << quadrant
		          << " -> " << targetQuadrant
		          << " Xcount=" << Xcount
		          << " Ycount=" << Ycount
		          << " Alt=" << Alt
		          << " HA=" << HA << "\n";
	}
}