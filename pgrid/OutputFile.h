#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "Config.h"
#include "Image.h"
#define FILE_OPEN_SUCCESS 0;
#define FILE_OPEN_CHECK_OVERWRITE 1;
#define FILE_OPEN_APPEND_TO_NONEXISTENT 2;
#define FILE_OPEN_EMPTY_PATH 3;

class OutputFile
{
private:
	ApplicationConfig* app_config;
	MeasurementConfig* measurement_config;
	GridConfig* grid_config;
	char filepath[100];
	//bool overwrite;
	bool saved;
	bool append;
	int neck;
	int seat_height;
	int seat_track;
	int img_last4;
	char img_description[50];

	std::ofstream outfile;
	//const char* neck;
	//const char* 
public:
	const char* neck_options_display[3] = { "5th Female", "50th Male", "95th Male" };
	const char* seat_height_options_display[3] = { "Down", "Mid", "Up" };
	const char* seat_track_options_display[3] = { "Full Forward", "Mid Track", "Full Rearward" };
	const char* neck_options_output[3] = { "5th_female", "50th_male", "95th_male" };
	const char* seat_height_options_output[3] = { "down", "mid", "up" };
	const char* seat_track_options_output[3] = { "forward", "mid", "rearward" };

	OutputFile(SessionConfig* session_config);

	bool file_exists();

	int open();

	void write_output(std::vector<cv::Point2f> data_points);

	void close();

	char* get_filepath_buf();

	char* get_img_description_buf();

	int* get_neck_ptr();
	int* get_seat_height_ptr();
	int* get_seat_track_ptr();

	void set_img_last4(int last4);

	int get_img_last4();

	bool* get_append_ptr();

	bool is_saved();

	void set_outfile_name(std::string newfilename);
};

