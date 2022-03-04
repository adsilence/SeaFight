#include "engine.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include <json.hpp> 
#include <spdlog/spdlog.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "utils.h"


Engine::Engine() {
	loadGameObjects();
}

Engine::~Engine() {}

void Engine::start() {
	double lastTime = glfwGetTime(), timer = lastTime;
	double deltaTime = 0, nowTime = 0;
	int frames = 0, updates = 0;
	const double delta = 1.0 / 120.0;

	while (!window.shouldClose()) {
		//get time
		nowTime = glfwGetTime();
		deltaTime += (nowTime - lastTime) / delta;
		lastTime = nowTime;

		//update at delta
		while (deltaTime >= 1.0) {
			update();
			updates++;
			deltaTime--;
		}

		render();
		frames++;

		//reset and output fps
		if (glfwGetTime() - timer > 1.0) {
			timer++;
			//std::cout << "FPS: " << frames << " Updates:" << updates << std::endl;
			updates = 0, frames = 0;
		}
	}

	vkDeviceWaitIdle(device.device());
}

void Engine::loadGameObjects() {
	std::vector<Sprite::Vertex> vertices{
	  {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	  {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
	auto sprite = std::make_shared<Sprite>(device, vertices);

	auto triangle = GameObject::createGameObject();
	triangle.sprite = sprite;
	triangle.color = { .1f, .1f, .8f };
	triangle.transform2d.translation.x = .2f;
	triangle.transform2d.scale = { 2.f, .5f };
	triangle.transform2d.rotation = 1;

	gameObjects.push_back(std::move(triangle));
}


void Engine::update() {
	glfwPollEvents();

	gameObjects[0].transform2d.rotation = sin(glfwGetTime());
}

void Engine::render() {
	if (auto commandBuffer = renderer.beginFrame()) {
      renderer.beginSwapchainRenderPass(commandBuffer);
      renderManager.renderGameObjects(commandBuffer, gameObjects);
      renderer.endSwapchainRenderPass(commandBuffer);
      renderer.endFrame();
    }}

void Engine::stop() {
	window.setWindowShouldClose();
}
