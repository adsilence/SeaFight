#pragma once 

#include "device.h"
#include "buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

class Sprite {
public:
	struct Vertex {
		glm::vec2 position;
		glm::vec3 color;
		glm::vec2 texCoord;

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};

	Sprite(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	~Sprite();

	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

private:
	Device& device;

	std::unique_ptr<Buffer> vertexBuffer;
	uint32_t vertexCount;
	std::unique_ptr<Buffer> indexBuffer;
	uint32_t indexCount;

	void createVertexBuffers(const std::vector<Vertex>& vertices);
	void createIndexBuffers(const std::vector<uint32_t>& indices);
};