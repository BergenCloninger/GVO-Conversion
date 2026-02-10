#include "GetQandYU.h"
#include "GlobalValues.h"
#include <cmath>
#include <algorithm>
#include <iostream>

double GetStime(); // you must implement this somewhere

void GetQandY(double ra, double dec, double& Alt, double& HA, double& Xcount, double& Ycount, int& q, int& y) {
    // disable timer (Pascal disables form Timer_Main)
    // Here we assume your timer system is independent; skip

    Meridian = GetStime();        // global
    EastHor = Meridian + 6.0;
    WestHor = Meridian - 6.0;

    if (WestHor < 0.0) WestHor += 24.0;
    if (EastHor > 24.0) EastHor -= 24.0;

    EastHA = -6;
    NorthHA = 12;
    WestHA = 6;
    SouthHA = 0;

    // Hour angle in hours
    HA = Meridian - ra;

    // Altitude (degrees)
    Alt = std::asin(std::sin(DToR * dec) * std::sin(DToR * C_Lat) + std::cos(DToR * dec) * std::cos(DToR * HA) * std::cos(DToR * C_Lat)); //use STD functions for trig calcs
    Alt *= RToD;

    // normalize HA between -12 and 12
    if (HA < -12.0) HA += 24.0;
    if (HA > 12.0) HA -= 24.0;

    // Determine quadrant and y-pole
    if (HA <= SouthHA && HA >= EastHA) {
        y = 1;
        q = 1;
    } else if (HA < EastHA && HA > -NorthHA) {
        y = -1;
        q = 2;
    } else if (HA >= SouthHA && HA <= WestHA) {
        y = 1;
        q = 3;
    } else if (HA > WestHA && HA < NorthHA) {
        y = -1;
        q = 4;
    } else {
        y = 0;
        q = 0;
    }

    // Dec counts: positive north of zenith, negative south
    if (dec >= C_Lat) {
        Ycount = (dec - C_Lat) * DECFACT;
    } else {
        Ycount = -1.0 * (C_Lat - dec) * DECFACT;
    }

    // RA counts: east of meridian negative, west positive
    Xcount = HA * 15.0 * RAFact;
}
