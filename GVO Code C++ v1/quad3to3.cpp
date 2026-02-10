#include "quad3to3.h"
#include "GlobalValues.h"
#include "CommUtils.h"

#include <cmath>
#include <iomanip>
#include <sstream>

void GoQuad3to3(double Xcount, double Ycount) {
    // Set motion flags
    movingRA = true;
    movingDEC = true;

    // RA
    RaPos = std::abs(Xcount);
    std::ostringstream ossRA;
    ossRA << std::fixed << std::setprecision(0) << RaPos;
    std::string CmdStr = ossRA.str();

    std::ostringstream ossTrk;
    ossTrk << std::fixed << TrkRate;
    std::string CmdStr2 = ossTrk.str();

    // Send RA STOP
    SendCommand("AX ST;");

    // Send RA SLEW
    CmdStr = "AX VL" + xvlslew + " MA-" + CmdStr + " GD ID;";
    SendCommand(CmdStr);

    // DEC
    decPos = Ycount;
    std::ostringstream ossDec;
    ossDec << std::fixed << std::setprecision(0) << decPos;
    CmdStr = ossDec.str();

    CmdStr = "AY VL" + yvlslew + " MA-" + CmdStr + " GD ID;";
    SendCommand(CmdStr);
}
