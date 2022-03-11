#include "renderManager.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

struct PushConstantData {
	glm::mat4 transform;
	alignas(16) glm::vec3 color;
};

RenderManager::RenderManager(Device& device, 
	VkRenderPass renderPass,
	std::vector<VkDescriptorSetLayout> setLayouts)
	: device{ device } {
	createPipelineLayout(setLayouts);
	createPipeline(renderPass);
}

RenderManager::~RenderManager() {
	vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}


void RenderManager::createPipelineLayout(std::vector<VkDescriptorSetLayout> setLayouts) {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PushConstantData);

	//std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ setLayout };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
	pipelineLayoutInfo.pSetLayouts = setLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
		VK_SUCCESS) {
		spdlog::critical("Failed to create pipeline layout!");
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void RenderManager::createPipeline(VkRenderPass renderPass) {
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	pipeline = std::make_unique<Pipeline>(
		device,
		"res/shaders/sprite.vert.spv",
		"res/shaders/sprite.frag.spv",
		pipelineConfig);
}

void RenderManager::renderGameObjects(
	VkCommandBuffer commandBuffer, 
	VkDescriptorSet descriptorSet,
	std::vector<GameObject>& gameObjects) {
	pipeline->bind(commandBuffer);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0,
		1,
		&descriptorSet,
		0,
		nullptr
	);

	for (auto& obj : gameObjects) {
		PushConstantData push{};
		push.color = obj.color;
		push.transform = obj.transform2d.mat4();

		vkCmdPushConstants(
			commandBuffer,
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PushConstantData),
			&push);
		obj.sprite->bind(commandBuffer);
		obj.sprite->draw(commandBuffer);
	}
}

