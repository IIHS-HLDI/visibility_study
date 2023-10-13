#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include "CameraPose.h"
#include "imgui_stdlib.h"

class CameraProfile
{
private:

	std::string file_path;

	std::string device; // e.g. Pixel 6 Pro
	
	std::string lens_descriptor; // e.g. Front facing camera 
	float zoom_level;
	//float pitch_angle; // zero if horizontal

	cv::Mat camera_intrinsic;
	cv::Mat dist_coeffs;

	float focal_length_mm;


public:
	std::string profile_descriptor; // e.g. Pixel 6 Pro Standard

	CameraProfile();

	int load_profile(const std::string& filepath);

	void write_profile(const std::string& output_file);

	void undistort(cv::Mat src_img, cv::Mat dst_img);

	std::vector<cv::Point2f> undistort_points(std::vector<cv::Point2f> src_pts);

	void calibrate(const std::string& input_file_dir, int* checkerboard_dims);
	void calibrate(const std::string& input_file_dir, std::vector<int> checkerboard_dims);

	std::vector<cv::Point2f> img_to_world_transform(std::vector<cv::Point2f> img_points, CameraPose camera_pose);

	float* get_focal_length_ptr();
	float* get_zoom_level_ptr();
	std::string* get_device_ptr();
	std::string* get_profile_descriptor_ptr();


	cv::Mat get_camera_matrix();
	cv::Mat get_dist_coeffs();
	//std::string* get_camera_ptr();
};

