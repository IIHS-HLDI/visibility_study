#pragma once
#include <GL/glew.h>
#include <GL/GL.h>
#include <glfw3.h>
#include <opencv2/core.hpp>
#include <vector>
#include "Config.h"
#include "Grid.h"
#include "CameraProfile.h"
#include "Image.h"

class Painter
{
private:
	GridConfig* grid_config;
	PainterConfig* paint_config;
	ImageConfig* img_config;
	ApplicationConfig* app_config;
	MeasurementConfig* measurement_config;

	std::vector<cv::Point2f> points;
	std::vector<cv::Point2f> projected_points_disp;
	

	const int view_radius = 6;
	const int crosshair_size = 2;

public:
	Painter(SessionConfig* session_config);
	std::vector<double> uv_coord_to_scene_coord(double u, double v);
	std::vector<cv::Point2f> uv_coords_to_scene_coords(std::vector<cv::Point2f>);

	std::vector<cv::Point2f> scene_to_uv_coord(std::vector<cv::Point2f> scene_points);

	std::vector<double> scene_to_uv_coord(double scene_x, double scene_y);

	void add_point(float x, float y);

	void add_point_at_click(float x, float y);

	void draw();

	void draw_ortho();

	std::vector<cv::Point2f> get_points();

	std::vector<cv::Point2f> project_points();

	void project_points_display();

	double distance(double x0, double y0,
		double x1, double y1);

	void erase(float x, float y);

	void clear_points();

	int size();
};

