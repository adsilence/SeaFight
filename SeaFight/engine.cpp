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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "inputManager.h"

struct SpriteUBO {
	glm::mat4 proj;
	glm::mat4 view;
};

Engine::Engine() {
	uboBuffers.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < uboBuffers.size(); i++) {
		uboBuffers[i] = std::make_unique<Buffer>(
			device,
			sizeof(SpriteUBO),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
		uboBuffers[i]->map();
	}

	spritePool = DescriptorPool::Builder(device).setMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT).addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Swapchain::MAX_FRAMES_IN_FLIGHT).build();

	auto setLayout = DescriptorSetLayout::Builder(device).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT).build();
	descriptorSets.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < descriptorSets.size(); i++) {
		auto bufferInfo = uboBuffers[i]->descriptorInfo();
		DescriptorWriter(*setLayout, *spritePool).writeBuffer(0, &bufferInfo).build(descriptorSets[i]);
	}
	renderManager = std::make_unique<RenderManager>(device, renderer.getSwapChainRenderPass(), setLayout->getDescriptorSetLayout());

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
	/*std::vector<Sprite::Vertex> vertices{
	  {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	  {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };*/
	const std::vector<Sprite::Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};
	const std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0
	};
	auto sprite = std::make_shared<Sprite>(device, vertices, indices);

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


	//temp translations
	gameObjects[0].transform2d.rotation = 90 * sin(glfwGetTime());
	if (InputManager::keys[GLFW_KEY_W]) {
		view = glm::translate(view, glm::vec3(0, 0.1f, 0));
	}
	if (InputManager::keys[GLFW_KEY_S]) {
		view = glm::translate(view, glm::vec3(0, -0.1f, 0));
	}
	if (InputManager::keys[GLFW_KEY_A]) {
		view = glm::translate(view, glm::vec3(0.1f, 0, 0));
	}
	if (InputManager::keys[GLFW_KEY_D]) {
		view = glm::translate(view, glm::vec3(-0.1f, 0, 0));
	}
}

void Engine::render() {
	if (auto commandBuffer = renderer.beginFrame()) {
		//update ubos
		SpriteUBO ubo{};
		//ubo.proj = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
		ubo.proj = glm::mat4(1.0f);
		ubo.view = view;
		//ubo.view = glm::mat4(1.0f);
		int frameIndex = renderer.getFrameIndex();
		uboBuffers[frameIndex]->writeToBuffer(&ubo);
		uboBuffers[frameIndex]->flush();

		//render frame
      renderer.beginSwapchainRenderPass(commandBuffer);
      renderManager->renderGameObjects(commandBuffer, descriptorSets[frameIndex], gameObjects);
      renderer.endSwapchainRenderPass(commandBuffer);
      renderer.endFrame();
    }}

void Engine::stop() {
	window.setWindowShouldClose();
}
