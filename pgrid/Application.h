#pragma once

#include <GL/glew.h>
#include <glfw3.h>
#include <GL/GL.h>

#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "nfd.h"

#include "ControlPanel.h"
#include "Config.h"
#include "PerspectivePanel.h"
#include "Painter.h"
#include "OrthoPanel.h"
#include "CalibrationMenu.h"
#include "NewProjectMenu.h"

#include <Windows.h>

class Application
{
private:
    // Pointer to GLFWwindow for event handling
	GLFWwindow* window;

    // Helper pointers to shared memory structures
	ApplicationConfig* app_config;
	ViewConfig* view_config;
	GridConfig* grid_config;
	ImageConfig* img_config;
	PainterConfig* paint_config;
	ViewConfig* ortho_config;

    // 
	int width, height;
	double mouse_x, mouse_y;

	double last_mouse_x, last_mouse_y;

	char window_label[100];
	
	ImGuiWindowFlags window_flags;
	ImGuiDockNodeFlags docknode_flags;
	ImGuiViewport* main_viewport;

	SessionConfig* session_config;
	
	ControlPanel ctrl_panel;
	PerspectivePanel perspective_panel;
	OrthoPanel ortho_panel;
	CalibrationMenu calibration_menu;
	NewProjectMenu new_project_menu;

	
public:
	Application(SessionConfig* session_config);

	std::string get_output_directory();

	void set_width(uint32_t new_width);
	void set_height(uint32_t new_height);

	static void glfw_error_callback(int error, const char* description);

	static void resize_callback(GLFWwindow* window, int new_width, int new_height);

	static void cursor_position_callback(GLFWwindow* window, double mouse_x, double mouse_y);

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	static void scroll_callback(GLFWwindow* window, double delta_x, double delta_y);

	static void keypress_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


	void import_image();

	void load_camera_profile();

	bool init();

	void main_loop();

	void close();

};

