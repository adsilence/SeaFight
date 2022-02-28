#pragma once

#include <memory>

#include <json.hpp>

#include "utils.h"
#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"

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
	Swapchain swapchain{ device, window.getExtent() };
	std::unique_ptr<Pipeline> pipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;

	void createPipelineLayout();
	void createPipeline();
	void createCommandBuffers();
};

