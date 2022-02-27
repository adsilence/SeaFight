#pragma once

#include "window.h"

class Engine {
public:
	int width = 800;
	int height = 600;

	Engine();
	~Engine();

	void start();

	void update();
	void render();

	void stop();

private:
	Window window{ width, height, "Sea Fight" };


};

