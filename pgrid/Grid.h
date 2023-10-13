#pragma once

#include <GL/glew.h>
#include <glfw3.h>

#include <GL/GL.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "GridCorner.h"
#include "Config.h"
#include "ReferencePoint.h"
#include <vector>

class Grid
{
private:
	SessionConfig* session_config;
	GridConfig* grid_config;
	ImageConfig* img_config;
	ApplicationConfig* app_config;

	std::vector<cv::Point2f> ref_points;
	std::vector<cv::Point2f> world_points;
	cv::Mat pM;
	cv::Mat pM_inv;

	//std::vector<ReferencePoint> ref_points;


public:
	std::vector<GridCorner> corner;

	Grid(SessionConfig* seession_config, float x0, float y0,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3);

	void draw();

	void draw_ortho();

	void compute_perspective_transform();

	cv::Mat get_perspective_transform();

	cv::Mat get_inverse_transform();

	float distance(float x0, float y0, float x1, float y1);

	int grab(float x, float y);
	
	void move_corner(int corner_idx,
		float x, float y);

	void nudge_corner(int corner_idx,
		float x, float y);

	void add_ref_point(double mx, double my);
};

