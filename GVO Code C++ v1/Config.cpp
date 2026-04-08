#include "Config.h"
#include "GlobalValues.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>

// Internal storage
static std::unordered_map<std::string, std::string> config;

void LoadConfig(const std::string& filename) {
	std::ifstream file(filename);

	if (!file) {
		std::cout << "Config: no config file found, using defaults\n";
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		// Skip empty lines and comments
		if (line.empty() || line[0] == '#')
			continue;

		auto pos = line.find('=');
		if (pos == std::string::npos)
			continue;

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		// Trim spaces (basic)
		while (!key.empty() && (key.back() == ' ' || key.back() == '\t')) key.pop_back();
		while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) value.erase(0, 1);

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

		if (config.count("DECFACT"))
			DECFACT = std::stod(config["DECFACT"]);

		if (config.count("C_Lat"))
			C_Lat = std::stod(config["C_Lat"]);

		if (config.count("C_Long"))
			C_Long = std::stod(config["C_Long"]);

		if (config.count("xvlslew"))
			xvlslew = config["xvlslew"];

		if (config.count("yvlslew"))
			yvlslew = config["yvlslew"];
		
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
	std::cout << "xvlslew: " << xvlslew << "\n";
	std::cout << "yvlslew: " << yvlslew << "\n";
	std::cout << "==============\n\n";
}