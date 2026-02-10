#include "Sync.h"
#include "CommUtils.h"
#include "CalcCoord.h"
#include "OMS68SERMC.h"
#include "GlobalValues.h"
#include "GetQandYU.h"
#include <windows.h>
#include <cmath>
#include <iostream>

void SyncScope() {
    if (movingRA || movingDEC) //don't sync while moving
        return;

    Coord* coord = CommUtils::GetCoordPtr();

    double Alt, HA, Xcount, Ycount;
    int quad;

    GetQandY(coord->RASync, coord->DecSync, Alt, HA, Xcount, Ycount, quad, yPole);

    if (Alt < 0.0) {
       	std::cerr << "Sync below Horizon\n";
        return;
    }

    // RA
    std::string CmdStr;
    std::string PreStrng;

    if (Xcount < 0)
        PreStrng = "AX LP-";
    else
        PreStrng = "AX LP";

    Xcount = std::abs(Xcount);
    CmdStr = PreStrng + std::to_string(static_cast<long>(Xcount)) + ";";
    SendCommand(CmdStr);

    // Back to tracking
    CmdStr = "AX JF" + std::to_string(TrkRate) + ";";
    SendCommand(CmdStr);

    // DEC
    if (Ycount < 0)
        PreStrng = "AY LP-";
    else
        PreStrng = "AY LP";

    Ycount = std::abs(Ycount);
    CmdStr = PreStrng + std::to_string(static_cast<long>(Ycount)) + ";";
    SendCommand(CmdStr);

    // Reset sync request
    coord->RASync = 0.0;
    coord->DecSync = 0.0;

    UpdateCoord();  // call your C++ function to refresh RA/Dec display
}
