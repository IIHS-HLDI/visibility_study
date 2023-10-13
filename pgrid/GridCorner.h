#pragma once
#include <glfw3.h>

#include <GL/GL.h>

class GridCorner
{
private:
	bool is_origin;
	const float corner_size = 50;

public:
	float x, y;

	GridCorner(float x_init, float y_init,
		bool set_origin);

	void make_origin();
	void clear_origin();
	void draw();

};



