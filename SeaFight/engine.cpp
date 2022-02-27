#include "engine.h"

#include <iostream>
#include <fstream>
#include <json.hpp> 
#include <spdlog/spdlog.h>

using json = nlohmann::json;

void load_settings() {
    json settings;

    std::ifstream i("res/settings.json");
    i >> settings;

    if (settings["dev_mode"] == true) {
        spdlog::set_level(spdlog::level::debug);
    }
    else {
        spdlog::set_level(spdlog::level::info);
    }
}

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::start()
{
}

void Engine::update()
{
}

void Engine::render()
{
}

void Engine::stop()
{
}
