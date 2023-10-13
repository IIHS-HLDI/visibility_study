#pragma once
#include <GL/glew.h>
#include <glfw3.h>

#include <GL/GL.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include <opencv2/imgcodecs/imgcodecs.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/aruco.hpp>
//#include <opencv2/aruco/aruco_calib.hpp>
#include "Config.h"
#include "CameraPose.h"
#include "CameraProfile.h"

class Image
{
private:
	const char* file_path;
	cv::Size img_size;
	cv::Mat raw_img;
	cv::Mat cv_img;
	cv::Mat cam_intrinsic;
	cv::Mat cam_rotation_translation;
	GLuint img_tex;
	ApplicationConfig* app_config;
	ImageConfig* img_config;
	GridConfig* grid_config;

public:
	Image(SessionConfig* session_config);
	
	void load();

	inline static bool exists(const std::string& name);
	inline static bool hasFile(const std::string& name);

	void compute_intrinsics();

	void render();

	void close();

	void find_markers();

	int get_last4();

	int get_width();
	int get_height();
	std::string get_filename();

};

