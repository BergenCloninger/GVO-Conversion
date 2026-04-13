#include "utils.h"
#include "GlobalValues.h"
#include <cmath>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream> // TODO: Remove this 

double SidTimeFract = 0.0;

// Helper: wrap angle to 0..2pi
double mod2pi(double x) {
	x = x - std::trunc(x / pi2) * pi2;
	if (x < 0) x += pi2;
	return x;
}

// Sidereal time at Greenwich 0h UT
double SiderealTime0(double T) {
	double day_term = T * 36525.0 + 0.5;
	double frac_part = day_term - std::floor(day_term);
	T = T - frac_part * 36525.0;

	double Theta = T * (36000.770053608 + T * (0.000387933 - T / 38710000.0));
	double TempReal = mod2pi((100.46061837 + Theta) * DToR);
	return TempReal * RToH;
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
	time_t t_now = time(nullptr);
	tm local_tm = *localtime(&t_now);

	double instant_here =
		local_tm.tm_hour +
		local_tm.tm_min / 60.0 +
		local_tm.tm_sec / 3600.0;

	int hour_offset = static_cast<int>(std::floor(C_Long / 15.0));

	// Copy local broken-down time first
	tm present_tm = local_tm;
	present_tm.tm_hour += hour_offset;

	// Normalize exactly once
	mktime(&present_tm);

	double instant_g =
		present_tm.tm_hour +
		present_tm.tm_min / 60.0 +
		present_tm.tm_sec / 3600.0;

	// Match Pascal: DST modifies InstantG only, not present_tm/date
	if (local_tm.tm_isdst > 0) {
		instant_g -= 1.0;
	}
	if (instant_g < 0.0) instant_g += 24.0;

	MeusDATE date;
	date.yy = present_tm.tm_year + 1900;
	date.mm = present_tm.tm_mon + 1;
	date.dd = present_tm.tm_mday;
	date.h = present_tm.tm_hour;
	date.m = present_tm.tm_min;
	date.s = present_tm.tm_sec;

	double JD = CalendarToJD(date);
	double UT = JDToT(JD);

	SidTime = SiderealTime0(UT);
	SidTime = SidTime + (1.00273790935 * instant_g);
	if (SidTime > 24.0) SidTime -= 24.0;

	SidTime = SidTime - (C_Long / 15.0);
	if (SidTime < 0.0) SidTime += 24.0;

	SidTimeFract = SidTime - instant_here;

	std::cout << "SetStime:"
			  << " InstantHere=" << instant_here
			  << " InstantG=" << instant_g
			  << " date=" << date.yy << "-" << date.mm << "-" << date.dd
			  << " SidTime=" << SidTime
			  << " SidTimeFract=" << SidTimeFract
			  << "\n";

	return SidTime;
}

double GetStime() {
	time_t t_now = time(nullptr);
	tm local_tm = *localtime(&t_now);

	double temp_real =
		local_tm.tm_hour +
		local_tm.tm_min / 60.0 +
		local_tm.tm_sec / 3600.0;

	double result = SidTimeFract + temp_real;
	if (result > 24.0) result -= 24.0;

	return result;
}