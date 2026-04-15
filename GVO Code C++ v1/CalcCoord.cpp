#include "CalcCoord.h"
#include "GlobalValues.h"
#include "GetQandYU.h"
#include "CommUtils.h"
#include "utils.h"

#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

inline double radToDeg(double rad) { return rad * 180.0 / pi; }
inline double degToRad(double deg) { return deg * pi / 180.0; }

void UpdateCoord() {
	Coord* coord = CommUtils::GetCoordPtr();

	if (!coord) {
		return;
	}

	static bool last_pp_failed = false;

	int pp_result = SendAndGetCommand(&CommRecord, "PP;", Response, sizeof(Response));

	if (!pp_result) {
		if (!last_pp_failed) {
			std::cerr << "UpdateCoord: failed to query controller with PP; "
					  << "(current code treats return value 0 as failure)\n";
			last_pp_failed = true;
		}
		return;
	}

	last_pp_failed = false;

	std::string resp = Response;
	std::stringstream ss(resp);
	std::string xStr, yStr;

	// Split the comma-separated response exactly like Pascal TempList.CommaText usage
	bool gotX = static_cast<bool>(std::getline(ss, xStr, ','));
	bool gotY = static_cast<bool>(std::getline(ss, yStr, ','));

	double x = 0.0;
	double y = 0.0;

	try {
		x = std::stod(xStr); // RA counts
		y = std::stod(yStr); // DEC counts
	}
	catch (const std::exception& e) {
		std::cerr << "UpdateCoord: could not parse PP response: [" << resp << "]\n";
		std::cerr << "UpdateCoord: exception = " << e.what() << "\n";
		return;
	}
	catch (...) {
		std::cerr << "UpdateCoord: could not parse PP response: [" << resp << "]\n";
		std::cerr << "UpdateCoord: unknown parse exception\n";
		return;
	}

	double Alt = 0.0;
	double HA = 0.0;
	double Xcount = 0.0;
	double Ycount = 0.0;
	double TempStime = 0.0;

	TempStime = GetStime();

	if (x == 0.0) {
		std::cerr << "ERROR: RA encoder returning 0: invalid state\n";
		return;
	}

	double originalXForRA = x;

	if ((x > 0.0) && (yPole > 0)) {				// quad 3
		RANow = TempStime - ((x / RAFact) / 15.0);
		if (RANow < 0.0)
			RANow += 24.0;
	}
	else if ((x < 0.0) && (yPole < 0)) {		// quad 2
		x = std::abs(x);
		RANow = TempStime + ((x / RAFact) / 15.0);
		if (RANow > 24.0)
			RANow -= 24.0;
	}
	else if ((x < 0.0) && (yPole > 0)) {		// quad 1
		x = std::abs(x);
		RANow = TempStime + ((x / RAFact) / 15.0);
		if (RANow > 24.0)
			RANow -= 24.0;
	}
	else if ((x > 0.0) && (yPole < 0)) {		// quad 4
		RANow = TempStime - ((x / RAFact) / 15.0);
		if (RANow < 0.0)
			RANow += 24.0;
	}
	else {
		std::cout << "UpdateCoord: WARNING: no RA branch matched!"
				  << " originalX=" << originalXForRA
				  << " currentX=" << x
				  << " yPole=" << yPole
				  << "\n";
	}

	RAHr = static_cast<int>(RANow);
	RAMin = static_cast<int>((RANow - RAHr) * 60.0);
	RASec = static_cast<int>((RANow - RAHr) * 3600.0 - (RAMin * 60.0));

	DECNow = C_Lat + (y / DECFACT);

	DECDeg = static_cast<int>(DECNow);
	DECMin = static_cast<int>((DECNow - DECDeg) * 60.0);
	DECSec = static_cast<int>((DECNow - DECDeg) * 3600.0 - (DECMin * 60.0));

	coord->RA = RANow;
	coord->Dec = DECNow;

	GetQandY(RANow, DECNow, Alt, HA, Xcount, Ycount, quadrant, yPole);

	// Safety stop below 30 degrees altitude, same as Pascal behavior
	if (Alt < 30.0) {
		SendCommand("AA ST;");   // stop all axes

		coord->RAGoto = 0.0;
		coord->DecGoto = 0.0;

		movingRA = false;
		movingDEC = false;

		std::cerr << "No track below 30 degrees!\n";
		lowAltitudeLockout = true;
		return;
	}
	
	altdeg = static_cast<int>(Alt);
	altmin = static_cast<int>((Alt - altdeg) * 60.0);
	altsec = static_cast<int>((Alt - altdeg) * 3600.0 - (altmin * 60.0));

	double azimuthArg = (-std::sin(degToRad(DECNow))) / std::cos(degToRad(C_Lat));

	if (azimuthArg < -1.0) azimuthArg = -1.0;
	if (azimuthArg > 1.0) azimuthArg = 1.0;

	double azimuth = radToDeg(std::acos(azimuthArg));

	azdeg = static_cast<int>(azimuth);
	azmin = static_cast<int>((azimuth - azdeg) * 60.0);
	azsec = static_cast<int>((azimuth - azdeg) * 3600.0 - (azmin * 60.0));
}