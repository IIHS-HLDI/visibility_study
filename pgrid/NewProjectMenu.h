#pragma once

#include <GL/glew.h>
#include "Config.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "nfd.h"
#include "SeatConfiguration.h"
#include <Windows.h>

const static char* vehicle_class_str[4] = {"Sedan", "SUV", "Minivan", "Pickup"};

class NewProjectMenu
{
private:
	ApplicationConfig* app_config;

	char project_file_path[100];

	std::string vehicle_make;
	std::string vehicle_model;
	std::string description;
	std::string comments;
	int vehicle_class;

	uint16_t vehicle_year;
	std::vector<SeatConfiguration> seat_configs;

public:
	NewProjectMenu(SessionConfig* config);

	void choose_project_file();

	void choose_image_files(SeatConfiguration* seat_config);

	void commit_settings();

	void add_seat_configuration(std::string neck_height,
		std::string seat_track,
		std::string seat_height,
		std::vector<std::string> image_filepaths);

	void add_seat_configuration();

	void layout();

};

