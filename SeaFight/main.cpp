#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "engine.h"
#include <spdlog/spdlog.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// entry for program start
// probably should put some sort of legal nonsense here
int main() {

    Engine engine{};
    try {
        engine.start();
    }
    catch (const std::exception& e) {
        spdlog::critical("{}", e.what());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
