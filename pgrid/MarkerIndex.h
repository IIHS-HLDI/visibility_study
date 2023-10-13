#pragma once
#include <glm/glm.hpp>
#include "Config.h"
#include <Windows.h>
#include <fstream>
#include <vector>
#include <opencv2/core/core.hpp>

class MarkerIndex
{
private:
	ApplicationConfig* app_config;
	char* index_filepath;
	std::vector<unsigned int> marker_ids;
	std::vector<float> x_coords; // TODO: figure out this thing
	std::vector<float> y_coords;
public:
	MarkerIndex(SessionConfig* session_config);
	void build_index();
	cv::Point2f lookup(unsigned int id);
};

