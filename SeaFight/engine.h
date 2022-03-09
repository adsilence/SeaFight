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
#include "descriptors.h"

//temp
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	std::unique_ptr<RenderManager> renderManager;

	std::vector<std::unique_ptr<Buffer>> uboBuffers;
	std::unique_ptr<DescriptorPool> spritePool;
	std::vector<VkDescriptorSet> descriptorSets;

	//temp
	glm::mat4 view = glm::mat4(1.0f);

	void loadGameObjects();
};

