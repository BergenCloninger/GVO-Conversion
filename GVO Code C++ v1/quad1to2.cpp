#include "quad1to2.h"
#include "GlobalValues.h"
#include "CommUtils.h" // for SendCommand

#include <cmath>
#include <iomanip>
#include <sstream>

void GoQuad1to2() {
    // Set motion flags
    movingRA = true;
    movingDEC = true;

    // RA calculation
    RaPos = RaTarget - EastHor;
    if (RaPos > 0.0) RaPos = 24.0 - RaPos;
    RaPos = std::abs(RaPos);
    RaPos = RaPos * RAFact * 15.0; // convert hours to counts

    // Convert RA to string
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << RaPos;
    std::string CmdStr = oss.str();

    std::ostringstream oss2;
    oss2 << std::fixed << TrkRate;
    std::string CmdStr2 = oss2.str();

    // Send RA STOP
    SendCommand("AX ST;");

    // Send RA SLEW
    CmdStr = "AX VL" + xvlslew + " MA" + CmdStr + " GD ID;";
    SendCommand(CmdStr);

    // DEC calculation
    if (DecTarget < 0.0)
        decPos = 90.0 + std::abs(DecTarget);
    else
        decPos = 90.0 - DecTarget;

    decPos = decPos * DECFACT;

    // Convert DEC to string
    std::ostringstream ossDec;
    ossDec << std::fixed << std::setprecision(0) << decPos;
    CmdStr = ossDec.str();

    // Send DEC SLEW
    CmdStr = "AY VL" + yvlslew + " MA" + CmdStr + " GD ID;";
    SendCommand(CmdStr);
}
