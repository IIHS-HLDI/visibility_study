/***********************************************************************
Copyright 2023 Insurance Institute for Highway Safety

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***********************************************************************/

#include <iostream>
#include <stdlib.h>
#include "Application.h"

#include <Windows.h>

using namespace std;



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	HMODULE module = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(module, path, MAX_PATH);
	
	char buffer[MAX_PATH + 1];
	int count = ::GetModuleFileNameA(NULL, buffer, MAX_PATH);
	string exe_path = string(buffer);
	exe_path.shrink_to_fit();
	exe_path = exe_path.substr(0, exe_path.size() - 9);

	SessionConfig* session_config = new SessionConfig;
	session_config->app_config = new ApplicationConfig;
	session_config->grid_config = new GridConfig;
	session_config->measurement_config = new MeasurementConfig;
	session_config->perspective_view_config = new ViewConfig;
	session_config->ortho_view_config = new ViewConfig;
	session_config->img_config = new ImageConfig;
	session_config->paint_config = new PainterConfig;

	errno_t err_img = strcpy_s(session_config->img_config->image_filepath, "C:\\");
	if (err_img != 0) {
		cout << "Error setting image file path" << endl;
	}
	session_config->img_config->cam_pose = new CameraPose;
	session_config->img_config->cam_pose->x_pos = 0;
	session_config->img_config->cam_pose->y_pos = 0;
	session_config->img_config->cam_pose->z_pos = 0;
	session_config->img_config->cam_pose->yaw_angle = 0;
	session_config->img_config->image_loaded = false;

	session_config->img_config->camera_profile = new CameraProfile;

	session_config->app_config->corner_control_scaler = 256;
	session_config->app_config->mode = 1;
	

	session_config->perspective_view_config->zoom = 1;
	session_config->perspective_view_config->zoom_speed = 0.01f;
	session_config->perspective_view_config->pan_x = 0;
	session_config->perspective_view_config->pan_y = 0;
	session_config->perspective_view_config->mouse_x = 0;
	session_config->perspective_view_config->mouse_y = 0;
	session_config->perspective_view_config->last_mouse_x = 0;
	session_config->perspective_view_config->last_mouse_y = 0;

	session_config->ortho_view_config->zoom = 1;
	session_config->ortho_view_config->zoom_speed = 0.01f;
	session_config->ortho_view_config->pan_x = 0;
	session_config->ortho_view_config->pan_y = 0;
	session_config->ortho_view_config->mouse_x = 0;
	session_config->ortho_view_config->mouse_y = 0;
	session_config->ortho_view_config->last_mouse_x = 0;
	session_config->ortho_view_config->last_mouse_y = 0;

	session_config->app_config->corner_control_sensitivity = 1;

	// set file path for marker index file
	string default_marker_index = exe_path.append("marker_index");
	errno_t err_mark = strcpy_s(session_config->app_config->marker_index_filepath, default_marker_index.c_str());
	if (err_mark != 0) {
		cout << "Error setting marker index file path" << endl;
	}

	session_config->grid_config->height = 15;
	session_config->grid_config->width = 8.5;
	session_config->grid_config->divs_x = 17;
	session_config->grid_config->divs_y = 30;
	session_config->grid_config->calibration_mode = 0;

	session_config->measurement_config->flip_x = false;
	session_config->measurement_config->flip_y = false;

	session_config->measurement_config->x_offset = 0;
	session_config->measurement_config->y_offset = 0;

	session_config->paint_config->paint_mode = 0;
	session_config->paint_config->erase_radius = 10;
	session_config->paint_config->paint_density_counter = 0;
	session_config->paint_config->paint_density_interval = 10;

	Application app(session_config);
	app.set_height(600);
	app.set_width(1000);

	// set file path and file name for output csv file
	errno_t err_out = strcpy_s(session_config->app_config->outfile_path, app.get_output_directory().c_str());
	if (err_out != 0) {
		cout << "Error setting output file path" << endl;
	}
	std::string default_outfile_name = "output.csv";
	session_config->app_config->outfile->set_outfile_name(default_outfile_name);

	if (!app.init()) {
		cout << "Application init error" << endl;
		return 1;
		//free(session_config);
	}
	cout << "Initialized" << endl;
	app.main_loop();
	app.close();
	//free(session_config);
	return 0;
}