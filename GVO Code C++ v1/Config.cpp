#include "Config.h"
#include "GlobalValues.h"

#include <fstream>
#include <unordered_map>
#include <iostream>
#include <cctype>

// Internal storage
static std::unordered_map<std::string, std::string> config;

static inline void TrimInPlace(std::string& s) {
	while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
		s.erase(s.begin());

	while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
		s.pop_back();
}

static inline void StripInlineComment(std::string& s) {
	auto pos = s.find('#');
	if (pos != std::string::npos)
		s = s.substr(0, pos);

	TrimInPlace(s);
}

void LoadConfig(const std::string& filename) {
	std::ifstream file(filename);

	if (!file) {
		std::cout << "Config: no config file found, using defaults\n";
		return;
	}

	config.clear();

	std::string line;
	while (std::getline(file, line)) {
		TrimInPlace(line);

		if (line.empty() || line[0] == '#')
			continue;

		auto pos = line.find('=');
		if (pos == std::string::npos)
			continue;

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		TrimInPlace(key);
		StripInlineComment(value);

		if (!key.empty())
			config[key] = value;
	}

	std::cout << "Config loaded from " << filename << "\n";
}

void ApplyConfig() {
	try {
		if (config.count("TrkRate"))
			TrkRate = std::stod(config["TrkRate"]);

		if (config.count("RAFact"))
			RAFact = std::stod(config["RAFact"]);

		if (config.count("C_Lat"))
			C_Lat = std::stod(config["C_Lat"]);

		if (config.count("C_Long"))
			C_Long = std::stod(config["C_Long"]);

		// Normal hand paddle
		if (config.count("handpad_ra_east_rate"))
			HandPadRaEastRate = std::stod(config["handpad_ra_east_rate"]);

		if (config.count("handpad_ra_west_rate"))
			HandPadRaWestRate = std::stod(config["handpad_ra_west_rate"]);

		if (config.count("handpad_dec_rate"))
			HandPadDecRate = std::stod(config["handpad_dec_rate"]);

		// DEC bump
		if (config.count("dec_bump_rate"))
			DecBumpRate = std::stoi(config["dec_bump_rate"]);

		if (config.count("dec_bump_ms"))
			DecBumpDurationMs = std::stoi(config["dec_bump_ms"]);

		// Fast paddle
		if (config.count("fastpad_x_slew"))
			FastPadXSlew = config["fastpad_x_slew"];

		if (config.count("fastpad_y_slew"))
			FastPadYSlew = config["fastpad_y_slew"];

		if (config.count("fastpad_x_fine"))
			FastPadXFine = config["fastpad_x_fine"];

		if (config.count("fastpad_y_fine"))
			FastPadYFine = config["fastpad_y_fine"];

		// Legacy / general axis values
		if (config.count("xvl"))
			xvl = config["xvl"];

		if (config.count("xac"))
			xac = config["xac"];

		if (config.count("yvl"))
			yvl = config["yvl"];

		if (config.count("yac"))
			yac = config["yac"];

		if (config.count("xacmax"))
			xacmax = config["xacmax"];

		if (config.count("xvlmax"))
			xvlmax = config["xvlmax"];

		if (config.count("yacmax"))
			yacmax = config["yacmax"];

		if (config.count("yvlmax"))
			yvlmax = config["yvlmax"];

		if (config.count("com_port"))
			ComPortNumber = std::stoi(config["com_port"]);

		if (config.count("oms_dll"))
			OmsDllPath = config["oms_dll"];

		if (config.count("teleapi_dll"))
			TeleApiDllPath = config["teleapi_dll"];

	} catch (...) {
		std::cout << "Config: error parsing values, using defaults where needed\n";
	}
}

void PrintConfig() {
	std::cout << "\n=== CONFIG ===\n";
	std::cout << "TrkRate: " << TrkRate << "\n";
	std::cout << "RAFact: " << RAFact << "\n";
	std::cout << "DECFACT: " << DECFACT << "\n";
	std::cout << "Lat: " << C_Lat << " Long: " << C_Long << "\n";

	std::cout << "Handpad RA east: " << HandPadRaEastRate << "\n";
	std::cout << "Handpad RA west: " << HandPadRaWestRate << "\n";
	std::cout << "Handpad DEC: " << HandPadDecRate << "\n";

	std::cout << "Fastpad slew X/Y: " << FastPadXSlew << " / " << FastPadYSlew << "\n";
	std::cout << "Fastpad fine X/Y: " << FastPadXFine << " / " << FastPadYFine << "\n";

	std::cout << "DEC bump rate: " << DecBumpRate << "\n";
	std::cout << "DEC bump ms: " << DecBumpDurationMs << "\n";

	std::cout << "xvl/xac: " << xvl << " / " << xac << "\n";
	std::cout << "yvl/yac: " << yvl << " / " << yac << "\n";
	std::cout << "xvlmax/xacmax: " << xvlmax << " / " << xacmax << "\n";
	std::cout << "yvlmax/yacmax: " << yvlmax << " / " << yacmax << "\n";
	std::cout << "==============\n\n";
}