#pragma once

#include <GL/glew.h>
#include "Config.h"
#include "imgui.h"
#include "CameraProfile.h"


// TODO: rename to profile builder
class CalibrationMenu
{
private:
	ApplicationConfig* app_config;
	CameraProfile camera_profile;
	char cal_img_dir[100];
	char save_file_path[100];
	int checkerboard_dims[2];

public:
	CalibrationMenu(SessionConfig* config);

	void choose_calibration_dir();

	void choose_save_file();

	void layout();

};

