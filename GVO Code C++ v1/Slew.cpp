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
    HWND thandle = 0;

    // Disable jog buttons if needed (optional for GUI)
    // mainform.sjog.enabled = false; etc.

    RaTarget = coord->RAGoto;
    DecTarget = coord->DecGoto;

    double Alt, HA, Xcount, Ycount;
    int i;

    GetQandY(RaTarget, DecTarget, Alt, HA, Xcount, Ycount, targetQuadrant, i);

    if (Alt < 20.0 || i == 0) {
        coord->RAGoto = 0.0;
        coord->DecGoto = 0.0;
		std::cout << "No slew below horizon";
        return;
    }

    // Slew based on target and current quadrant
    if (targetQuadrant == 1 && quadrant == 1)
        GoQuad1to1(Xcount, Ycount);
    else if (targetQuadrant == 3 && quadrant == 1)
        GoQuad1to3(Xcount, Ycount);
    else if (targetQuadrant == 3 && quadrant == 3)
        GoQuad3to3(Xcount, Ycount);
    else if (targetQuadrant == 1 && quadrant == 3)
        GoQuad3to1(Xcount, Ycount);
    else {
        coord->RAGoto = 0.0;
        coord->DecGoto = 0.0;
		std::cout << "No slew below pole";
    }

    // Note: Other quadrant transitions (4-to-*) are commented out in Pascal ????
}
