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
	std::cout << "\n========== UpdateCoord BEGIN ==========\n";

	Coord* coord = CommUtils::GetCoordPtr();

	if (!coord) {
		std::cerr << "UpdateCoord: Shared memory not initialized!\n";
		std::cout << "========== UpdateCoord END (no coord) ==========\n";
		return;
	}

	std::cout << std::fixed << std::setprecision(6);
	std::cout << "UpdateCoord: coord ptr = " << coord << "\n";
	std::cout << "UpdateCoord: shared memory before update:"
			  << " RA=" << coord->RA
			  << " Dec=" << coord->Dec
			  << " RAGoto=" << coord->RAGoto
			  << " DecGoto=" << coord->DecGoto
			  << " RASync=" << coord->RASync
			  << " DecSync=" << coord->DecSync
			  << "\n";

	std::cout << "UpdateCoord: globals before update:"
			  << " yPole=" << yPole
			  << " quadrant=" << quadrant
			  << " RAFact=" << RAFact
			  << " DECFACT=" << DECFACT
			  << " C_Lat=" << C_Lat
			  << "\n";

	static bool last_pp_failed = false;

	// IMPORTANT:
	// Pascal keeps retrying until SendAndGetString returns 0:
	// While (SendAndGetString(...) <> 0) Do ;
	//
	// That strongly suggests success == 0.
	// If your SendAndGetCommand follows the same convention,
	// then this condition should probably be:
	//
	// if (SendAndGetCommand(...) != 0) { failure }
	//
	// For today's testing, we log the raw return value so you can verify it.
	int pp_result = SendAndGetCommand(&CommRecord, "PP;", Response, sizeof(Response));

	std::cout << "UpdateCoord: SendAndGetCommand(\"PP;\") returned " << pp_result << "\n";
	std::cout << "UpdateCoord: raw Response buffer after PP = [" << Response << "]\n";

	// Current logic preserved exactly as written in your version for now:
	if (!pp_result) {
		if (!last_pp_failed) {
			std::cerr << "UpdateCoord: failed to query controller with PP; "
					  << "(current code treats return value 0 as failure)\n";
			last_pp_failed = true;
		}
		std::cout << "========== UpdateCoord END (PP failure branch) ==========\n";
		return;
	}

	last_pp_failed = false;

	std::string resp = Response;
	std::stringstream ss(resp);
	std::string xStr, yStr;

	std::cout << "UpdateCoord: PP response copied to std::string = [" << resp << "]\n";
	std::cout << "UpdateCoord: PP response length = " << resp.size() << "\n";

	// Split the comma-separated response exactly like Pascal TempList.CommaText usage
	bool gotX = static_cast<bool>(std::getline(ss, xStr, ','));
	bool gotY = static_cast<bool>(std::getline(ss, yStr, ','));

	std::cout << "UpdateCoord: getline results: gotX=" << gotX << " gotY=" << gotY << "\n";
	std::cout << "UpdateCoord: xStr = [" << xStr << "]\n";
	std::cout << "UpdateCoord: yStr = [" << yStr << "]\n";

	double x = 0.0;
	double y = 0.0;

	try {
		x = std::stod(xStr); // RA counts
		y = std::stod(yStr); // DEC counts
		std::cout << "UpdateCoord: parsed counts successfully:"
				  << " x=" << x
				  << " y=" << y
				  << "\n";
	}
	catch (const std::exception& e) {
		std::cerr << "UpdateCoord: could not parse PP response: [" << resp << "]\n";
		std::cerr << "UpdateCoord: exception = " << e.what() << "\n";
		std::cout << "========== UpdateCoord END (parse failure) ==========\n";
		return;
	}
	catch (...) {
		std::cerr << "UpdateCoord: could not parse PP response: [" << resp << "]\n";
		std::cerr << "UpdateCoord: unknown parse exception\n";
		std::cout << "========== UpdateCoord END (parse failure) ==========\n";
		return;
	}

	double Alt = 0.0;
	double HA = 0.0;
	double Xcount = 0.0;
	double Ycount = 0.0;
	double TempStime = 0.0;

	TempStime = GetStime();

	std::cout << "UpdateCoord: GetStime() = " << TempStime << "\n";

	if (x == 0.0) {
		std::cout << "UpdateCoord: x was 0.0, applying Pascal quirk x=1.0\n";
		x = 1.0;
	}

	// ================= RA calculation =================
	double originalXForRA = x;

	if ((x > 0.0) && (yPole > 0)) {				// quad 3
		std::cout << "UpdateCoord: RA branch -> quad 3 logic (x>0 && yPole>0)\n";
		RANow = TempStime - ((x / RAFact) / 15.0);
		if (RANow < 0.0)
			RANow += 24.0;
	}
	else if ((x < 0.0) && (yPole < 0)) {		// quad 2
		std::cout << "UpdateCoord: RA branch -> quad 2 logic (x<0 && yPole<0)\n";
		x = std::abs(x);
		RANow = TempStime + ((x / RAFact) / 15.0);
		if (RANow > 24.0)
			RANow -= 24.0;
	}
	else if ((x < 0.0) && (yPole > 0)) {		// quad 1
		std::cout << "UpdateCoord: RA branch -> quad 1 logic (x<0 && yPole>0)\n";
		x = std::abs(x);
		RANow = TempStime + ((x / RAFact) / 15.0);
		if (RANow > 24.0)
			RANow -= 24.0;
	}
	else if ((x > 0.0) && (yPole < 0)) {		// quad 4
		std::cout << "UpdateCoord: RA branch -> quad 4 logic (x>0 && yPole<0)\n";
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

	std::cout << "UpdateCoord: RA calc details:"
			  << " originalX=" << originalXForRA
			  << " effectiveX=" << x
			  << " RAFact=" << RAFact
			  << " TempStime=" << TempStime
			  << " RANow=" << RANow
			  << "\n";

	RAHr = static_cast<int>(RANow);
	RAMin = static_cast<int>((RANow - RAHr) * 60.0);
	RASec = static_cast<int>((RANow - RAHr) * 3600.0 - (RAMin * 60.0));

	std::cout << "UpdateCoord: RA HMS:"
			  << " " << RAHr << "h"
			  << " " << RAMin << "m"
			  << " " << RASec << "s"
			  << "\n";

	// ================= DEC calculation =================
	DECNow = C_Lat + (y / DECFACT);

	std::cout << "UpdateCoord: DEC calc details:"
			  << " C_Lat=" << C_Lat
			  << " y=" << y
			  << " DECFACT=" << DECFACT
			  << " DECNow=" << DECNow
			  << "\n";

	DECDeg = static_cast<int>(DECNow);
	DECMin = static_cast<int>((DECNow - DECDeg) * 60.0);
	DECSec = static_cast<int>((DECNow - DECDeg) * 3600.0 - (DECMin * 60.0));

	std::cout << "UpdateCoord: DEC DMS:"
			  << " " << DECDeg << "d"
			  << " " << DECMin << "m"
			  << " " << DECSec << "s"
			  << "\n";

	coord->RA = RANow;
	coord->Dec = DECNow;

	std::cout << "UpdateCoord: wrote shared memory:"
			  << " RA=" << coord->RA
			  << " Dec=" << coord->Dec
			  << "\n";

	std::cout << "UpdateCoord: calling GetQandY with:"
			  << " RANow=" << RANow
			  << " DECNow=" << DECNow
			  << " incoming quadrant=" << quadrant
			  << " incoming yPole=" << yPole
			  << "\n";

	GetQandY(RANow, DECNow, Alt, HA, Xcount, Ycount, quadrant, yPole);

	std::cout << "UpdateCoord: GetQandY returned:"
			  << " Alt=" << Alt
			  << " HA=" << HA
			  << " Xcount=" << Xcount
			  << " Ycount=" << Ycount
			  << " quadrant=" << quadrant
			  << " yPole=" << yPole
			  << "\n";

	// Safety stop below 30 degrees altitude, same as Pascal behavior
	if (Alt < 30.0) {
		std::cout << "UpdateCoord: Alt < 30.0, sending safety stop command\n";
		SendCommand("AA ST;");
		coord->RAGoto = 0.0;
		coord->DecGoto = 0.0;
		std::cerr << "No track below 30 degrees!\n";
	}
	else {
		std::cout << "UpdateCoord: Altitude is safe for tracking\n";
	}

	altdeg = static_cast<int>(Alt);
	altmin = static_cast<int>((Alt - altdeg) * 60.0);
	altsec = static_cast<int>((Alt - altdeg) * 3600.0 - (altmin * 60.0));

	std::cout << "UpdateCoord: Alt DMS:"
			  << " " << altdeg << "d"
			  << " " << altmin << "m"
			  << " " << altsec << "s"
			  << "\n";

	double azimuthArg = (-std::sin(degToRad(DECNow))) / std::cos(degToRad(C_Lat));
	std::cout << "UpdateCoord: azimuth raw arg before clamp = " << azimuthArg << "\n";

	if (azimuthArg < -1.0) azimuthArg = -1.0;
	if (azimuthArg > 1.0) azimuthArg = 1.0;

	double azimuth = radToDeg(std::acos(azimuthArg));

	azdeg = static_cast<int>(azimuth);
	azmin = static_cast<int>((azimuth - azdeg) * 60.0);
	azsec = static_cast<int>((azimuth - azdeg) * 3600.0 - (azmin * 60.0));

	std::cout << "UpdateCoord: azimuth final = " << azimuth << "\n";
	std::cout << "UpdateCoord: Az DMS:"
			  << " " << azdeg << "d"
			  << " " << azmin << "m"
			  << " " << azsec << "s"
			  << "\n";

	std::cout << "UpdateCoord: final shared memory:"
			  << " RA=" << coord->RA
			  << " Dec=" << coord->Dec
			  << " RAGoto=" << coord->RAGoto
			  << " DecGoto=" << coord->DecGoto
			  << " RASync=" << coord->RASync
			  << " DecSync=" << coord->DecSync
			  << "\n";

	std::cout << "=========== UpdateCoord END ===========\n";
}