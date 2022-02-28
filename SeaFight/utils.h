#pragma once

#include <iostream>
#include <fstream>

#include <json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

// namespace to manage the game settings 
// settings are stored in settings.json file in the res folder
// settings can be modified anywhere 
class Settings {
public:
	inline static json settings;

	Settings() {
		std::ifstream in("res/settings.json");
		in >> settings;

		if (settings["dev_mode"] == true) {
			spdlog::set_level(spdlog::level::debug);
		}
		else {
			spdlog::set_level(spdlog::level::info);
		}
	}

	void writeSettings() {
		std::ofstream out("res/settings.json");
		out << settings;
	}

};
