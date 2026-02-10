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

    SendCommand("AX ST;");

    CmdStr = "AX VL" + xvlslew + " MA" + CmdStr + " GD ID;";
    SendCommand(CmdStr);

    if (DecTarget < 0.0)
        decPos = 90.0 + std::abs(DecTarget);
    else
        decPos = 90.0 - DecTarget;

    decPos = decPos * DECFACT;

    std::ostringstream ossDec;
    ossDec << std::fixed << std::setprecision(0) << decPos;
    CmdStr = ossDec.str();

    CmdStr = "AY VL" + yvlslew + " MA" + CmdStr + " GD ID;"; //1:1 command string needs testing
    SendCommand(CmdStr);
}
