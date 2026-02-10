#include "CalcCoord.h"
#include "GlobalValues.h"
#include "GetQandYU.h"
#include "CommUtils.h"
#include "utils.h"
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>

// helper
inline double radToDeg(double rad) { return rad * 180.0 / pi; }
inline double degToRad(double deg) { return deg * pi / 180.0; }

void UpdateCoord() {
    if (!CommUtils::pndomem) {
        std::cerr << "Shared memory not initialized!\n";
        return;
    }

    Coord* coord = CommUtils::GetCoordPtr();
    if (!coord) return;

    double x = 0.0, y = 0.0, Alt = 0.0, HA = 0.0, Xcount = 0.0, Ycount = 0.0, TempStime = 0.0;
    int quad = 0;

    // Simulate getting stepper counts from mount
    // In Pascal: While SendAndGetString(CommRecord,'PP;',Response)<> 0 Do
    // We'll assume you have a function for that, here we just use coord->RA/Dec counts
    x = coord->RA;    // RA counts
    y = coord->Dec;   // DEC counts

    // get sidereal time
    TempStime = GetStime();  // implement this to return current sidereal time

    if (x == 0.0) x = 1.0;

    // RA calculation by quadrant (idk why it's like this I should change it)
    if (x > 0.0 && yPole > 0) { // quad 3
        RANow = TempStime - (x / RAFact / 15.0);
        if (RANow < 0) RANow += 24.0;
    }
    else if (x < 0.0 && yPole < 0) { // quad 2
        RANow = TempStime + (std::abs(x) / RAFact / 15.0);
        if (RANow > 24.0) RANow -= 24.0;
    }
    else if (x < 0.0 && yPole > 0) { // quad 1
        RANow = TempStime + (std::abs(x) / RAFact / 15.0);
        if (RANow > 24.0) RANow -= 24.0;
    }
    else if (x > 0.0 && yPole < 0) { // quad 4
        RANow = TempStime - (x / RAFact / 15.0);
        if (RANow < 0) RANow += 24.0;
    }

    RAHr = static_cast<int>(RANow);
    RAMin = static_cast<int>((RANow - RAHr) * 60);
    RASec = static_cast<int>((RANow - RAHr) * 3600 - RAMin * 60);

    // DEC calculation
    DECNow = C_Lat + (y / DECFACT); // stepper counts positive north, negative south
    DECDeg = static_cast<int>(DECNow);
    DECMin = static_cast<int>((DECNow - DECDeg) * 60);
    DECSec = static_cast<int>((DECNow - DECDeg) * 3600 - DECMin * 60);

    // update shared memory (coord struct is global)
    coord->RA = RANow;
    coord->Dec = DECNow;

    // get quadrant, counts, and yPole
    GetQandY(RANow, DECNow, Alt, HA, Xcount, Ycount, quadrant, yPole);

    if (Alt < 20.0) {
        SendCommand("AA ST;");
        Coord* c = CommUtils::GetCoordPtr();
        if (c) { c->RAGoto = 0.0; c->DecGoto = 0.0; }
        std::cerr << "No track below horizon!\n"; // below horizon safety
    }

    double azimuth = (-std::sin(degToRad(DECNow))) / std::cos(degToRad(C_Lat));
    if (azimuth < -1.0) azimuth = -1.0;
    if (azimuth > 1.0) azimuth = 1.0;
    azimuth = radToDeg(std::acos(azimuth));

    altdeg = static_cast<int>(Alt);
    altmin = static_cast<int>((Alt - altdeg) * 60);
    altsec = static_cast<int>((Alt - altdeg) * 3600 - altmin * 60);

    azdeg = static_cast<int>(azimuth);
    azmin = static_cast<int>((azimuth - azdeg) * 60);
    azsec = static_cast<int>((azimuth - azdeg) * 3600 - azmin * 60);

    // Optional debug
    //std::cout << "RA: " << RANow << " DEC: " << DECNow << " Alt: " << Alt << " Az: " << azimuth << "\n";
}
