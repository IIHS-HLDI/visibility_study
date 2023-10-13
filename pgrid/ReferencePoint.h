#pragma once
#include <GL/glew.h>
#include <GL/GL.h>
#include <glfw3.h>
#include <imgui.h>
#include "Config.h"

class ReferencePoint
{
private:
	GridConfig* grid_config;

	double x, y;
	int grid_x, grid_y;
	//int grid_coords;
	int idx;

public:
	float ref_x, ref_y;

	int grid_coords[2];

	ReferencePoint(SessionConfig* session_config, double x, double y, int idx);

	void draw();

	double get_x();

	double get_y();

	int get_grid_x();

	int get_grid_y();

	float get_ref_x();
	
	float get_ref_y();

	int* grid_x_ptr();

	int* grid_y_ptr();

	void set_grid_coords(int x, int y);

	void set_world_coords(float x, float y);
};

