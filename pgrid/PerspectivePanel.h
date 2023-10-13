#pragma once

#include <GL/glew.h>
#include <windows.h>
#include <glfw3.h>
#include <GL/GL.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include "Config.h"
#include "Grid.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Image.h"
#include "Camera2D.h"
#include "Painter.h"

/**
* The PerspectivePanel class encapsulates the Perspective Panel window 
*/
class PerspectivePanel
{
private:

	GLFWwindow* window; // Pointer to main GLFWwindow
	
	// Pointers to shared session memory
	ApplicationConfig* app_config;
	ViewConfig* view_config;
	GridConfig* grid_config;

	// width and height and position of the perspective panel
	float width, height;
	ImVec2 window_pos;


	// Image shown in the panel 
	Image image;

	// 2D camera
	Camera2D camera;

	// Perspective grid
	Grid grid;

	// OpenGL framebuffer memory
	GLuint fbo;
	GLuint tex;
	GLuint rbo;

public:

	PerspectivePanel(SessionConfig* session_config);

	void set_window_pointer(GLFWwindow* window);

	void init();

	void load_image();

	void render();

	void close();

	float get_pos_x();

	float get_pos_y();

	glm::dvec2 mouse_to_scene_pos(double mx, double my);

	bool is_mouse_on(double mx, double my);

	std::vector < std::vector<cv::Point2f>> image_to_scene_pos(std::vector<std::vector<cv::Point2f>> quad_points);

};

