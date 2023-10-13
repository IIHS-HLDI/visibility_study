#pragma once

#include <vector>
#include "Config.h"

const static char* neck_heights[3] = {
	"5th Female",
	"50th Male",
	"95th Male"
};

const static char* seat_tracks[3] = {
	"rearmost",
	"mid",
	"foremost" };

const static char* seat_heights[3] = {
	"up",
	"mid",
	"down",
};

typedef struct {
	int neck_height;
	int seat_track;
	int seat_height;

	std::vector<std::string> image_filepaths;
	std::vector<SessionConfig> configs;
} SeatConfiguration;