#include "engine.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include <json.hpp> 
#include <spdlog/spdlog.h>

#include "utils.h"

Engine::Engine() {
	loadSprites();
	createPipelineLayout();
	recreateSwapChain();
	createCommandBuffers();
}

Engine::~Engine() {
	vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

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

void Engine::loadSprites() {
	std::vector<Sprite::Vertex> vertices {
      {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};	
	sprite = std::make_unique<Sprite>(device, vertices);
}

void Engine::createPipelineLayout() {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void Engine::recreateSwapChain() {
auto extent = window.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = window.getExtent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(device.device());

  if (swapchain == nullptr) {
    swapchain = std::make_unique<Swapchain>(device, extent);
  } else {
    swapchain = std::make_unique<Swapchain>(device, extent, std::move(swapchain));
    if (swapchain->imageCount() != commandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }

  createPipeline();
}

void Engine::createPipeline() {
  assert(swapchain != nullptr && "Cannot create pipeline before swap chain");
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = swapchain->getRenderPass();
	pipelineConfig.pipelineLayout = pipelineLayout;
	pipeline = std::make_unique<Pipeline>(
		device,
		"res/shaders/sprite.vert.spv",
		"res/shaders/sprite.frag.spv",
		pipelineConfig);
}

void Engine::createCommandBuffers() {
	commandBuffers.resize(swapchain->imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = device.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void Engine::freeCommandBuffers() {
	  vkFreeCommandBuffers(
      device.device(),
      device.getCommandPool(),
      static_cast<uint32_t>(commandBuffers.size()),
      commandBuffers.data());
  commandBuffers.clear();
}

void Engine::recordCommandBuffer(int imageIndex) {
	
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			spdlog::critical("Failed to begin recording command buffer!");
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain->getRenderPass();
		renderPassInfo.framebuffer = swapchain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(swapchain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{{0, 0}, swapchain->getSwapChainExtent()};
	vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		pipeline->bind(commandBuffers[imageIndex]);
		sprite->bind(commandBuffers[imageIndex]);
		sprite->draw(commandBuffers[imageIndex]);
	
		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}


void Engine::update() {
	glfwPollEvents();
}

void Engine::render() {
  uint32_t imageIndex;
  auto result = swapchain->acquireNextImage(&imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    spdlog::critical("Failed to acquire swap chain image!");
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  recordCommandBuffer(imageIndex);
  result = swapchain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      window.windowResized()) {
    window.resetWindowResizedFlag();
    recreateSwapChain();
    return;
  } else if (result != VK_SUCCESS) {
    spdlog::critical("Failed to present swap chain image!");
    throw std::runtime_error("Failed to present swap chain image!");
  }
}

void Engine::stop() {
	window.setWindowShouldClose();
}
