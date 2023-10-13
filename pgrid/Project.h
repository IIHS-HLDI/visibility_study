#pragma once

#include <string>
#include "SeatConfiguration.h"

class Project
{
private:
	unsigned int vehicle_year;

	std::string vehicle_make;
	std::string vehicle_model;
	std::string vehicle_trim;
	std::string vehicle_comment;

	SeatConfiguration seat_configurations[];
public:

	Project();

	Project(
		int year,
		std::string make,
		std::string model,
		std::string trim,
		std::string comment,
		SeatConfiguration seat_configurations[]);

	void serialize(const std::string& output_filepath);

	void deserialize(const std::string& input_filepath);

	unsigned int get_vehicle_year();
	std::string get_vehicle_make();
	std::string get_vehicle_model();
	std::string get_vehicle_trim();
	std::string get_vehicle_comment();
};

