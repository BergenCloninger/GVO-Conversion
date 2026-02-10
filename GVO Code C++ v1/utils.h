#pragma once
#include <ctime>
#include <string>

// Forward-declare global constants (from global_values.h)
extern double C_Long;  // longitude of your site in degrees

// Struct for date/time
struct MeusDATE {
    int yy, mm;
    double dd;
    int h, m;
    double s;
};

// Sidereal time functions
double SiderealTime(double T);
double SiderealTime0(double T);

// Calendar / Julian Day functions
double CalendarToJD(const MeusDATE& date);
double JDToT(double JD);

// Local sidereal time functions
double SetStime();
double GetStime();

// Helper
double mod2pi(double x);

//format helper function
std::string FormatDouble(double v, int width, int precision);