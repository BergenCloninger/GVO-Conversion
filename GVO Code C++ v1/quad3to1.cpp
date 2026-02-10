#include "quad3to1.h"
#include "GlobalValues.h"
#include "CommUtils.h"

#include <cmath>
#include <iomanip>
#include <sstream>

void GoQuad3to1(double Xcount, double Ycount) {
    // Set motion flags
    movingRA = true;
    movingDEC = true;

    // RA
    RaPos = Xcount;  // positive
    std::ostringstream ossRA;
    ossRA << std::fixed << std::setprecision(0) << RaPos;
    std::string CmdStr = ossRA.str();

    std::ostringstream ossTrk;
    ossTrk << std::fixed << TrkRate;
    std::string CmdStr2 = ossTrk.str();

    // Send RA STOP
    SendCommand("AX ST;");

    // Send RA SLEW
    CmdStr = "AX VL" + xvlslew + " MA" + CmdStr + " GD ID;";
    SendCommand(CmdStr);

    // DEC
    decPos = std::abs(Ycount);
    std::ostringstream ossDec;
    ossDec << std::fixed << std::setprecision(0) << decPos;
    CmdStr = ossDec.str();

    if (DecTarget < C_Lat) {
        CmdStr = "AY VL" + yvlslew + " MA-" + CmdStr + " GD ID;";
    } else {
        CmdStr = "AY VL" + yvlslew + " MA" + CmdStr + " GD ID;";
    }

    SendCommand(CmdStr);
}