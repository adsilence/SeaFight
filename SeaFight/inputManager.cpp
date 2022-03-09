#include "inputManager.h"

#include <cmath>
#include <iostream>

bool InputManager::keys[350];

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	keys[key] = action;
}
