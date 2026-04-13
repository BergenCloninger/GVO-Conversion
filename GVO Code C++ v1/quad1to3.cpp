#include "quad1to3.h"
#include "GlobalValues.h"
#include "CommUtils.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>

bool GoQuad1to3(double Xcount, double Ycount, double decTarget) {
	movingRA = true;
	movingDEC = true;

	std::string CmdStr;
	std::string CmdStr1;
	std::string CmdStr2;

	// RaPos := xCount;     // is positive
	RaPos = Xcount;

	// Str(Rapos:14:0,CmdStr);
	{
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(0) << std::setw(14) << RaPos;
		CmdStr = oss.str();
	}

	// CmdStr := TrimLeft(CmdStr);
	{
		size_t first = CmdStr.find_first_not_of(' ');
		CmdStr = (first == std::string::npos) ? "" : CmdStr.substr(first);
	}

	// Str(TrkRate,CmdStr2);
	{
		std::ostringstream oss;
		oss << TrkRate;
		CmdStr2 = oss.str();
	}

	// CmdStr1 := 'AX ST;';
	CmdStr1 = "AX ST;";

	std::cout << "[GoQuad1to3 exact Pascal] sending raw CmdStr instead of CmdStr1: [" << CmdStr << "]\n";
	SendCommand(CmdStr);

	// CmdStr := 'AX  VL'+xvlslew+' MA'+ CmdStr + ' GD ID; ' ;
	CmdStr = "AX  VL" + xvlslew + " MA" + CmdStr + " GD ID; ";

	std::cout << "GoQuad1to3:\n";
	std::cout << "  Xcount=" << Xcount << " Ycount=" << Ycount << "\n";
	std::cout << "  decTarget=" << decTarget << " C_Lat=" << C_Lat << "\n";
	std::cout << "  RA raw count string: [" << CmdStr << "]\n";

	// SendString(CommRecord,pchar(CmdStr));
	bool raOk = SendCommand(CmdStr);
	std::cout << "[GoQuad1to3] after AX send, raOk=" << raOk << "\n";
	if (!raOk) {
		std::cout << "Failed to send RA slew command\n";
		return false;
	}

	// DecPos := abs(Ycount);
	decPos = std::abs(Ycount);

	// Str(Decpos:14:0,CmdStr);
	{
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(0) << std::setw(14) << decPos;
		CmdStr = oss.str();
	}

	// CmdStr := TrimLeft(CmdStr);
	{
		size_t first = CmdStr.find_first_not_of(' ');
		CmdStr = (first == std::string::npos) ? "" : CmdStr.substr(first);
	}

	// if ( DecTarget < c_lat) then ...
	if (decTarget < C_Lat) {
		CmdStr = "AY VL" + yvlslew + " MA-" + CmdStr + " GD ID;";
	} else {
		CmdStr = "AY VL" + yvlslew + " MA" + CmdStr + " GD ID;";
	}

	std::cout << "  DEC cmd: [" << CmdStr << "]\n";

	bool decOk = SendCommand(CmdStr);
	std::cout << "[GoQuad1to3] after AY send, decOk=" << decOk << "\n";
	if (!decOk) {
		std::cout << "Failed to send DEC slew command\n";
		return false;
	}

	return true;
}