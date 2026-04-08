#include "CalcCoord.h"
#include "GlobalValues.h"
#include "GetQandYU.h"
#include "CommUtils.h"
#include "utils.h"

#include <cmath>
#include <iostream>
#include <string>
#include <sstream>

inline double radToDeg(double rad) { return rad * 180.0 / pi; }
inline double degToRad(double deg) { return deg * pi / 180.0; }

void UpdateCoord() {
	Coord* coord = CommUtils::GetCoordPtr();

	if (!coord) {
		std::cerr << "Shared memory not initialized!\n";
		return;
	}

	// Expected format is comma-separated, matching Pascal TempList.CommaText usage
	std::string resp = Response;
	std::stringstream ss(resp);
	std::string xStr, yStr;

	static bool last_pp_failed = false;

    if (!SendAndGetCommand(&CommRecord, "PP;", Response, sizeof(Response))) {
        if (!last_pp_failed) {
            std::cerr << "UpdateCoord: failed to query controller with PP;\n";
            last_pp_failed = true;
        }
        return;
    }

    last_pp_failed = false;

	double x = 0.0;
	double y = 0.0;

	try {
		x = std::stod(xStr); // RA counts
		y = std::stod(yStr); // DEC counts
	}
	catch (...) {
		std::cerr << "UpdateCoord: could not parse PP response: [" << resp << "]\n";
		return;
	}

	double Alt = 0.0;
	double HA = 0.0;
	double Xcount = 0.0;
	double Ycount = 0.0;
	double TempStime = 0.0;

	TempStime = GetStime();

	if (x == 0.0)
		x = 1.0;

	// ================= RA calculation =================
	// Matches Pascal quadrant logic based on x sign and yPole
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

	RAHr = static_cast<int>(RANow);
	RAMin = static_cast<int>((RANow - RAHr) * 60.0);
	RASec = static_cast<int>((RANow - RAHr) * 3600.0 - (RAMin * 60.0));

	// ================= DEC calculation =================
	// Pascal: DECNow := C_Lat + (Y / DecFact)
	DECNow = C_Lat + (y / DECFACT);

	DECDeg = static_cast<int>(DECNow);
	DECMin = static_cast<int>((DECNow - DECDeg) * 60.0);
	DECSec = static_cast<int>((DECNow - DECDeg) * 3600.0 - (DECMin * 60.0));

	coord->RA = RANow;
	coord->Dec = DECNow;

	GetQandY(RANow, DECNow, Alt, HA, Xcount, Ycount, quadrant, yPole);

	// Safety stop below 30 degrees altitude, same as Pascal behavior
	if (Alt < 30.0) {
		SendCommand("AA ST;");
		coord->RAGoto = 0.0;
		coord->DecGoto = 0.0;
		std::cerr << "No track below 30 degrees!\n";
	}

	altdeg = static_cast<int>(Alt);
	altmin = static_cast<int>((Alt - altdeg) * 60.0);
	altsec = static_cast<int>((Alt - altdeg) * 3600.0 - (altmin * 60.0));

	// Keep conversion helpers here because DECNow and C_Lat are degree-based in this port
	double azimuth = (-std::sin(degToRad(DECNow))) / std::cos(degToRad(C_Lat));
	if (azimuth < -1.0) azimuth = -1.0;
	if (azimuth > 1.0) azimuth = 1.0;
	azimuth = radToDeg(std::acos(azimuth));

	azdeg = static_cast<int>(azimuth);
	azmin = static_cast<int>((azimuth - azdeg) * 60.0);
	azsec = static_cast<int>((azimuth - azdeg) * 3600.0 - (azmin * 60.0));

    std::cout << "Alt=" << Alt << "Dec=" << DECNow << " Az=" << azimuth << "\n"; // Optional debug print

	// Optional debug
	// std::cout << "PP=[" << resp << "] RA=" << RANow << " DEC=" << DECNow
	//           << " Alt=" << Alt << " Az=" << azimuth << "\n";
}