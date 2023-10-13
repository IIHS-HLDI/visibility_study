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

#include "OutputFile.h"
OutputFile::OutputFile(SessionConfig* session_config) {

	app_config = session_config->app_config;
	measurement_config = session_config->measurement_config;
	grid_config = session_config->grid_config;
	//img_last4 = app_config->image->get_last4();
	//overwrite = 1;
	append = false;
	saved = false;
	img_last4 = 0;
	neck = 0;
	seat_height = 0;
	seat_track = 0;
}

bool OutputFile::file_exists() {
	struct stat buffer;
	return (stat(app_config->outfile_path, &buffer) == 0);
}

int OutputFile::open() {
	if (!app_config->outfile_path) {
		return FILE_OPEN_EMPTY_PATH;
	}

	if (!append) {
		if (file_exists()) {
			return FILE_OPEN_CHECK_OVERWRITE;
		}

		outfile.open(app_config->outfile_path, std::ios::out | std::ios::trunc);
		return FILE_OPEN_SUCCESS;
	}

	if (!file_exists()) {
		return FILE_OPEN_APPEND_TO_NONEXISTENT;
	}
	
	outfile.open(app_config->outfile_path, std::ios::out | std::ios::app);
	return FILE_OPEN_SUCCESS;
}

void OutputFile::write_output(std::vector<cv::Point2f> data_points) {
	
	if (grid_config->calibration_mode == 0) {
		float flip_x = 1;
		float flip_y = 1;
		if (measurement_config->flip_x) {
			flip_x = -1;
		}
		if (measurement_config->flip_y) {
			flip_y = -1;
		}
		for (int i = 0; i < data_points.size(); i++) {
			data_points[i].x = flip_x * (data_points[i].x - measurement_config->x_offset);
			data_points[i].y = flip_y * (data_points[i].y - measurement_config->y_offset);
		}
	}
	else {
		for (int i = 0; i < data_points.size(); i++) {
			data_points[i].x = data_points[i].x;
			data_points[i].y = data_points[i].y;
		}
	}

	if (!append) {
		outfile << "x,y,description,img_last4,neck,seat_track,seat_height" << std::endl;
	}
	for (unsigned int i = 0; i < data_points.size(); i++) {
		outfile << std::fixed << data_points[i].x << "," <<
			std::fixed << data_points[i].y << "," <<
			img_description << "," <<
			std::setw(4) << std::setfill('0') << img_last4 << "," <<
			neck_options_output[neck] << "," <<
			seat_track_options_output[seat_track] << "," <<
			seat_height_options_output[seat_height] << std::endl;
	}
}

void OutputFile::close() {
	outfile.close();
}

char* OutputFile::get_filepath_buf() {
	return filepath;
}

char* OutputFile::get_img_description_buf() {
	return img_description;
}
int* OutputFile::get_neck_ptr() {
	return &neck;
}
int* OutputFile::get_seat_height_ptr() {
	return &seat_height;
}
int* OutputFile::get_seat_track_ptr() {
	return &seat_track;
}

void OutputFile::set_img_last4(int last4) {
	img_last4 = last4;
}

int OutputFile::get_img_last4() {
	return img_last4;
}

bool* OutputFile::get_append_ptr() {
	return &append;
}

bool OutputFile::is_saved() {
	return saved;
}

void OutputFile::set_outfile_name(std::string new_filename) {
	if (app_config->outfile_path) {
		std::string fpath = app_config->outfile_path;
		// find the last / or \ to determine the location of the file name in the path
		size_t last_slash = fpath.find_last_of("/\\");
		// replace the file name with the new file name
		std::string new_csv_filename = new_filename.substr(0, new_filename.find_last_of(".")) + ".csv";
		std::string new_path = fpath.substr(0, last_slash + 1) + new_csv_filename;
		// set the new file path and name
		strcpy_s(app_config->outfile_path, new_path.c_str());
		strcpy_s(app_config->outfile_name, new_csv_filename.c_str());
	}
}