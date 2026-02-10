#include "utils.h"
#include "GlobalValues.h"
#include <cmath>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <string>

double SidTimeFract = 0.0;

// Helper: wrap angle to 0..2pi
double mod2pi(double x) {
    x = x - std::floor(x / pi2) * pi2;
    if (x < 0) x += pi2;
    return x;
}

// Sidereal time at Greenwich 0h UT
double SiderealTime0(double T) {
    T = T - std::floor(T * 36525.0 + 0.5) * 36525.0;  // T at 0h
    double Theta = T * (36000.770053608 + T * (0.000387933 - T / 38710000.0));
    double TempReal = mod2pi((100.46061837 + Theta) * DToR);
    return TempReal * RToH;  // radians -> hours
}

// Sidereal time at Greenwich at any UT
double SiderealTime(double T) {
    double Theta = T * (360.98564736629 * 36525.0 + T * (0.000387933 - T / 38710000.0));
    double TempReal = mod2pi((280.46061837 + Theta) * DToR);
    return TempReal * RToH;
}

// Julian Day from calendar
double CalendarToJD(const MeusDATE& date) {
    int y = date.yy;
    int m = date.mm;
    double D = date.dd;

    if (m <= 2) {
        y -= 1;
        m += 12;
    }

    int A = y / 100;
    int B = 0;
    if (date.yy > 1582 || (date.yy == 1582 && (date.mm > 10 || (date.mm == 10 && date.dd > 4)))) {
        B = 2 - A + A / 4;
    }

    return std::floor(365.25 * (y + 4716)) + std::floor(30.6001 * (m + 1)) + D + B - 1524.5;
}

std::string FormatDouble(double v, int width, int precision)
{
	std::ostringstream oss;

	oss << std::fixed
	    << std::setw(width)
	    << std::setprecision(precision)
	    << v;

	return oss.str();
}

// Julian centuries since J2000
double JDToT(double JD) {
    return (JD - 2451545.0) / 36525.0;
}

// Set local sidereal time
double SetStime() {
    using namespace std::chrono;

    auto now = system_clock::now();
    time_t t_now = system_clock::to_time_t(now);
    tm local_tm = *localtime(&t_now);

	tm utc_tm;
	gmtime_s(&utc_tm, &t_now);

	double InstantHere = local_tm.tm_hour + local_tm.tm_min / 60.0 + local_tm.tm_sec / 3600.0;
	double InstantG   = utc_tm.tm_hour + utc_tm.tm_min / 60.0 + utc_tm.tm_sec / 3600.0;

    if (InstantG < 0) InstantG += 24;
    if (InstantG >= 24) InstantG -= 24;

    MeusDATE date;
    date.yy = local_tm.tm_year + 1900;
    date.mm = local_tm.tm_mon + 1;
    date.dd = local_tm.tm_mday;
    date.h = local_tm.tm_hour;
    date.m = local_tm.tm_min;
    date.s = local_tm.tm_sec;

    double JD = CalendarToJD(date);
    double UT = JDToT(JD);

    double SidTime = SiderealTime0(UT);
    SidTime += 1.00273790935 * InstantG;
    if (SidTime > 24) SidTime -= 24.0;

    // Convert to local longitude
    SidTime -= C_Long / 15.0;
    if (SidTime < 0) SidTime += 24.0;
	SidTimeFract = SidTime - InstantHere;
    return SidTime;
}

// Get current local sidereal time
double GetStime() {
    using namespace std::chrono;

    auto now = system_clock::now();
    time_t t_now = system_clock::to_time_t(now);
    tm local_tm = *localtime(&t_now);

    double InstantHere = local_tm.tm_hour + local_tm.tm_min / 60.0 + local_tm.tm_sec / 3600.0;
    double result = SidTimeFract + InstantHere;
    if (result >= 24.0) result -= 24.0;
    return result;
}
