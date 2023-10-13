#pragma once
#include <GL/glew.h>
#include "Config.h"
#include "imgui.h"
#include "Grid.h"
#include "OutputFile.h"

class ControlPanel
{
	ApplicationConfig* app_config;
	ViewConfig* view_config;
	GridConfig* grid_config;
	MeasurementConfig* measurement_config;
	ImageConfig* img_config;
	PainterConfig* paint_config;

public:
	ControlPanel(SessionConfig* config);
	
	void layout();
	void choose_camera_profile();
	void choose_image_file();
	void choose_output_file();
	void choose_calibration_dir();
};

