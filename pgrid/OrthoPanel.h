#pragma once

#include <GL/glew.h>
#include <glfw3.h>
#include <GL/GL.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include "Config.h"
#include "Grid.h"
#include "Camera2D.h"
#include "Painter.h"

class OrthoPanel
{
private:
	GLFWwindow* window;
	ApplicationConfig* app_config;
	ViewConfig* view_config;
	GridConfig* grid_config;

	float width, height;
	Camera2D camera;

	GLuint fbo;
	GLuint tex;
	GLuint rbo;

	ImVec2 window_pos;

public:
	OrthoPanel(SessionConfig* session_config);

	void set_window_pointer(GLFWwindow* window);

	void init();
	void render();
	void close();

	bool is_mouse_on(double mx, double my);

};

