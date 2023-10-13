#pragma once
#include <GL/glew.h>
#include <GL/GL.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Config.h"

class Camera2D
{
private:

	ViewConfig* view_config;

	double width;
	double height;
	double x, y;
	double scale;

	const double ppu = 1.0;

	glm::vec2 center;

	glm::vec3 camera_pos;
	glm::vec3 camera_target;
	glm::vec3 camera_direction;

	glm::vec3 up;
	glm::vec3 camera_right;
	glm::vec3 camera_up;

public:
	Camera2D(ViewConfig* view_config);

	void apply_cam();

	void update();

	void set_width(double w);
	void set_height(double h);
	
	void set_x(double x_pos);
	void set_y(double y_pos);
	void set_scale(double scale);

	glm::dvec2 mouse_to_scene_coords(double u, double v);

};

