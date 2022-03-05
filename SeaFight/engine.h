#pragma once

#include <memory>

#include <json.hpp>

#include "utils.h"
#include "window.h"
#include "device.h"
#include "gameobject.h"
#include "renderer.h"
#include "renderManager.h"
#include "buffer.h"

// main program loop and high level manager
class Engine {
public:
	int width;
	int height;

	Engine();
	~Engine();

	void start();

	void update();
	void render();

	void stop();

private:
	Settings settings{};
	Window window{Settings::settings["window_width"], Settings::settings["window_height"], "Sea Fight"};
	Device device{ window };
	std::vector<GameObject> gameObjects;
	Renderer renderer{ window, device };
	RenderManager renderManager{ device, renderer.getSwapChainRenderPass() };

	std::vector<std::unique_ptr<Buffer>> uboBuffers;

	void loadGameObjects();
};

