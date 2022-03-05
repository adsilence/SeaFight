#pragma once

#include "device.h"
#include "gameobject.h"
#include "pipeline.h"
#include "utils.h"

#include <memory>
#include <vector>

class RenderManager {
public:
	RenderManager(Device& device, VkRenderPass renderPass);
	~RenderManager();

	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;

	void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects);

private:
	Device& device;

	std::unique_ptr<Pipeline> pipeline;
	VkPipelineLayout pipelineLayout;

	void createPipelineLayout();
	void createPipeline(VkRenderPass renderPass);
};