#include "quad1to1.h"
#include "GlobalValues.h"
#include "CommUtils.h" // for SendCommand

#include <cmath>
#include <iomanip>
#include <sstream>

void GoQuad1to1(double Xcount, double Ycount) {
	movingRA = true;
    movingDEC = true;

    // RA
    RaPos = Xcount;
    RaPos = std::abs(RaPos);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << RaPos;
    std::string CmdStr = oss.str();

    std::ostringstream oss2;
    oss2 << std::fixed << TrkRate;
    std::string CmdStr2 = oss2.str();

    // Send RA STOP
    SendCommand("AX ST;");

    // Send RA SLEW
    CmdStr = "AX VL" + xvlslew + " MA-" + CmdStr + " GD ID;";
    SendCommand(CmdStr);

    // DEC
    decPos = Ycount;
    std::ostringstream ossDec;
    ossDec << std::fixed << std::setprecision(0) << std::abs(decPos);
    CmdStr = ossDec.str();

    if (decPos < 0.0) {
        CmdStr = "AY VL" + yvlslew + " MA-" + CmdStr + " GD ID;";
    } else {
        CmdStr = "AY VL" + yvlslew + " MA" + CmdStr + " GD ID;";
    }

    SendCommand(CmdStr);
}
