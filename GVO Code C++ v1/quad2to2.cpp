#include "quad2to2.h"
#include "GlobalValues.h"
#include "CommUtils.h"

#include <cmath>
#include <iomanip>
#include <sstream>

void GoQuad2to2() {
    // Set motion flags
    movingRA = true;
    movingDEC = true;

    // RA calculation
    RaPos = RaTarget - EastHor;
    if (RaPos > 0.0) RaPos = 24.0 - RaPos;
    RaPos = std::abs(RaPos);
    RaPos = (RaPos * RAFact) * 15.0;

    std::ostringstream ossRA;
    ossRA << std::fixed << std::setprecision(0) << RaPos;
    std::string CmdStr = ossRA.str();

    std::ostringstream ossTrk;
    ossTrk << std::fixed << TrkRate;
    std::string CmdStr2 = ossTrk.str();

    SendCommand("AX ST;");

    CmdStr = "AX VL" + xvlslew + " MA" + CmdStr + " GD ID;";
    SendCommand(CmdStr);

    decPos = 90.0 - DecTarget;
    if (DecTarget < 0.0) decPos = 90.0 + std::abs(DecTarget);
    decPos = decPos * DECFACT;

    std::ostringstream ossDec;
    ossDec << std::fixed << std::setprecision(0) << decPos;
    CmdStr = ossDec.str();

    CmdStr = "AY VL" + yvlslew + " MA" + CmdStr + " GD ID;";
    SendCommand(CmdStr);
}
