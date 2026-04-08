#include "GetQandYU.h"
#include "GlobalValues.h"
#include <cmath>

double GetStime();

void GetQandY(double ra, double dec, double& Alt, double& HA, double& Xcount, double& Ycount, int& q, int& y) {
	Meridian = GetStime();
	EastHor = Meridian + 6.0;
	WestHor = Meridian - 6.0;

	if (WestHor < 0.0) WestHor += 24.0;
	if (EastHor > 24.0) EastHor -= 24.0;

	EastHA = -6.0;
	NorthHA = 12.0;
	WestHA = 6.0;
	SouthHA = 0.0;

	// Altitude uses HA in degrees, exactly like Pascal
	double HAdeg = 15.0 * (Meridian - ra);

	Alt = std::asin(
		(std::sin(DToR * dec) * std::sin(DToR * C_Lat)) +
		(std::cos(DToR * dec) * std::cos(DToR * HAdeg) * std::cos(DToR * C_Lat))
	);
	Alt *= RToD;

	// Ypole / quadrant uses HA in hours, exactly like Pascal
	HA = Meridian - ra;

	if (HA < -12.0) HA += 24.0;
	if (HA > 12.0) HA -= 24.0;

	if ((HA <= SouthHA) && (HA >= EastHA)) {
		y = 1;
		q = 1;
	}
	else if ((HA < EastHA) && (HA > -NorthHA)) {
		y = -1;
		q = 2;
	}
	else if ((HA >= SouthHA) && (HA <= WestHA)) {
		y = 1;
		q = 3;
	}
	else if ((HA > WestHA) && (HA < NorthHA)) {
		y = -1;
		q = 4;
	}
	else {
		y = 0;
		q = 0;
	}

	if (dec >= C_Lat) {
		Ycount = (dec - C_Lat) * DECFACT;
	}
	else {
		Ycount = -1.0 * ((C_Lat - dec) * DECFACT);
	}

	Xcount = (HA * 15.0) * RAFact;
}