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

#include "Image.h"
#include <string>
#include <regex>
#include <vector>
#include "OutputFile.h"
#include "PerspectivePanel.h"
#include "MarkerIndex.h"

Image::Image(SessionConfig* session_config) {
	app_config = session_config->app_config;
	img_config = session_config->img_config;
	grid_config = session_config->grid_config;
}

/**
 * Checks that a file exists
 *
 * @param name filepath of target file
 *
 * @return true if file at pathname exists
 *
 */
inline bool Image::exists(const std::string & name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

/**
 * Checks that a file name is selected
 *
 * @param name filepath of target file
 *
 * @return true if file is part of the path name
 *
 */
inline bool Image::hasFile(const std::string& name) {
	std::string fpath = name;
	std::string base_file = fpath.substr(fpath.find_last_of("/\\") + 1);
	std::string no_ext = base_file.substr(0, fpath.find(".") - 1);
	return (base_file != "" && no_ext != "");
}

//void Image::add_image_point(float x, float y) {
//
//}


void Image::load() {

	app_config->image = this;
	this->file_path = img_config->image_filepath;

	if (!exists(file_path)) {
		std::cout << "CANNOT FIND FILE" << std::endl;
		return;
	}

	if (!hasFile(file_path)) {
		std::cout << "NO FILE SELECTED" << std::endl;
		return;
	}

	app_config->outfile->set_img_last4(this->get_last4());
	app_config->outfile->set_outfile_name(this->get_filename());

	cv_img = cv::imread(file_path, cv::IMREAD_COLOR);

	if (cv_img.empty()) {
		std::cout << "file empty" << std::endl;
		return;
	}
	img_config->image_loaded = true;
	//cv::Mat undistorted;
	//cv::undistort(cv_img, undistorted, img_config->camera_profile->get_camera_matrix(), img_config->camera_profile->get_dist_coeffs());
	//undistorted.copyTo(cv_img);
	cv_img.copyTo(raw_img);


	//cv_img.
	cv::cvtColor(cv_img, cv_img, cv::COLOR_BGR2RGBA);
	cv::flip(cv_img, cv_img, 0);
	//cv::imshow("wooder", cv_img);
	//cv::waitKey(0);


	img_size = cv_img.size();

	glGenTextures(1, &img_tex);
	glBindTexture(GL_TEXTURE_2D, img_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



}

void Image::render() {
	glEnable(GL_TEXTURE_2D);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cv_img.cols, cv_img.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, cv_img.ptr());
	glGenerateMipmap(GL_TEXTURE_2D);
	
	float half_width = img_size.width / 2;
	float half_height = img_size.height / 2;

	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(-half_width, -half_height);
		glTexCoord2i(0, 1); glVertex2i(-half_width, half_height);
		glTexCoord2i(1, 1); glVertex2i(half_width, half_height);
		glTexCoord2i(1, 0); glVertex2i(half_width, -half_height);
	glEnd();

	
	glDisable(GL_TEXTURE_2D);


	static int view_radius = 5;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < img_config->corners.size(); i++) {
		for (unsigned int j = 0; j < img_config->corners[i].size(); j++) {
			glBegin(GL_POLYGON);
				glVertex2f(img_config->corners[i][j].x - view_radius, img_config->corners[i][j].y - view_radius);
				glVertex2f(img_config->corners[i][j].x - view_radius, img_config->corners[i][j].y + view_radius);
				glVertex2f(img_config->corners[i][j].x + view_radius, img_config->corners[i][j].y + view_radius);
				glVertex2f(img_config->corners[i][j].x + view_radius, img_config->corners[i][j].y - view_radius);
			glEnd();
		}
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void Image::close() {
	glDeleteTextures(1, &img_tex);
}

void Image::find_markers() {
	cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();

	// Setings for aruco marker detection
	// 
	//detectorParams.polygonalApproxAccuracyRate = 0.1;
	//detectorParams.minCornerDistanceRate = 0.01;
	//detectorParams.minMarkerDistanceRate = 0.01;
	detectorParams.perspectiveRemovePixelPerCell = 10;
	//detectorParams.perspectiveRemoveIgnoredMarginPerCell = 0.3;
	//detectorParams.maxErroneousBitsInBorderRate = 0.50;
	detectorParams.cornerRefinementMethod = cv::aruco::CORNER_REFINE_CONTOUR;
	detectorParams.cornerRefinementMinAccuracy = 0.01;
	detectorParams.cornerRefinementMaxIterations = 5000;
	
	// IF YOU ARE USING MARKERS WITH DIFFERENT GRID SIZES LIKE 6X6 or 7X7 THIS MUST BE CHANGED TO DICT_NXN_50
	cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
	cv::aruco::ArucoDetector detector(dictionary, detectorParams);
	
	std::vector<int> ids;
	std::vector<std::vector<cv::Point2f>> corners, rejected;
	detector.detectMarkers(raw_img, corners, ids, rejected);
	if (ids.size() > 0) {
		img_config->ids = ids;
		img_config->corners = app_config->perspective_panel->image_to_scene_pos(corners);
		for (int i = 0; i < img_config->corners.size(); i++) {
			img_config->scene_points.push_back(img_config->corners[i][3]);
			img_config->scene_points.push_back(img_config->corners[i][0]);
			img_config->scene_points.push_back(img_config->corners[i][1]);
			img_config->scene_points.push_back(img_config->corners[i][2]);
			cv::Point2f world_point = app_config->marker_index->lookup(img_config->ids[i]);
			cv::Point2f corner0 = cv::Point2f(world_point.x, world_point.y + 0.30);
			cv::Point2f corner1 = cv::Point2f(world_point.x + 0.30, world_point.y + 0.30);
			cv::Point2f corner2 = cv::Point2f(world_point.x + 0.30, world_point.y);
			img_config->world_points.push_back(world_point);
			img_config->world_points.push_back(corner0);
			img_config->world_points.push_back(corner1);
			img_config->world_points.push_back(corner2);
		}
	//	cv::aruco::drawDetectedMarkers(raw_img, corners, ids);
	}

	//raw_img.copyTo(cv_img);
	//cv::Mat image_copy = cv_img.clone();
	//
	//cv_img.copyTo(image_copy);
	//
	//cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
	//cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_7X7_250);
	//cv::aruco::ArucoDetector detector(dictionary, detectorParams);
	//
	//std::vector<int> ids;
	//std::vector<std::vector<cv::Point2f>> corners, rejected;
	//ids.size();
	//detector.detectMarkers(image_copy, corners, ids, rejected);

	//detector.detectMarkers(cv_img, corners, ids, rejected);

	//if (ids.size() > 0) {
		//cv::aruco::drawDetectedMarkers(cv_img, corners, ids);
	//}
}

/**
* Gets last 4 digits of image name (without the extension)
*/
int Image::get_last4() {
	if (file_path == NULL) {
		return -9999;
	}
	std::string fpath = file_path;
	std::string base_file = fpath.substr(fpath.find_last_of("/\\") + 1);
	std::string no_ext = base_file.substr(0, fpath.find(".")-1);

	std::string last4 = fpath.substr(fpath.find_last_of(".") - 4, fpath.find_last_of("."));
	return atoi(last4.c_str());
}

std::string Image::get_filename() {
	if (file_path == NULL) {
		return nullptr;
	}
	std::string fpath = file_path;
	std::string base_file = fpath.substr(fpath.find_last_of("/\\") + 1);
	std::string no_ext = base_file.substr(0, fpath.find_last_of(".") - 1);
	return no_ext.c_str();
}

int Image::get_width() {
	return img_size.width;
}

int Image::get_height() {
	return img_size.height;
}
