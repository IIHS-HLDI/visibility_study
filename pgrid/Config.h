#pragma once
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <glm/glm.hpp>
#include "CameraPose.h"
//#include "Grid.h"

class Grid;
class ReferencePoint;
class Painter;
class Image;
class OutputFile;
class MarkerIndex;
class PerspectivePanel;
class CameraProfile;

enum app_mode {
	GRID,
	PAINT,
};

static const char* modes[3] = { "Point Calibrate", "Grid", "Paint"};

static const char* grid_calibration_modes[4] = {"Corners", "Point Calibrate", "Marker Calibrate", "Markerless"};

typedef struct {
	char image_filepath[256];
	
	CameraPose* cam_pose;
	CameraProfile* camera_profile;


	std::vector<int> ids;
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<cv::Point2f> scene_points;
	std::vector<cv::Point2f> world_points;

	//template<class Archive>
	//void serialize(Archive& archive)
	//{
	//	archive(image_filepath, ids, corners, scene_points, world_points); // serialize things by passing them to the archive
	//}

	bool image_loaded;
}ImageConfig;

typedef struct {
	double zoom;
	float zoom_speed;
	float pan_x;
	float pan_y;
	double mouse_x;
	double mouse_y;
	double last_mouse_x;
	double last_mouse_y;
	double scene_x;
	double scene_y;

	//template<class Archive>
	//void serialize(Archive& archive)
	//{
	//	archive(zoom, zoom_speed, pan_x, pan_y); // serialize things by passing them to the archive
	//}
} ViewConfig;

typedef struct {
	int mode;
	//char* mode_str[2];

	char outfile_path[256];
	char outfile_name[50];
	char marker_index_filepath[256];

	float corner_control_sensitivity;
	float corner_control_scaler;

	Image* image;
	Painter* painter;
	OutputFile* outfile;
	MarkerIndex* marker_index;
	PerspectivePanel* perspective_panel;

	//template<class Archive>
	//void serialize(Archive& archive)
	//{
	//	archive(mode, outfile_path, marker_index_filepath,
	//		corner_control_sensitivity, corner_control_scaler); // serialize things by passing them to the archive
	//}
} ApplicationConfig;

typedef struct {
	Grid* grid;
	float height;
	float width;
	int divs_x;
	int divs_y;
	float grid_interval_x;
	float grid_interval_y;
	int calibration_mode;
	std::vector<ReferencePoint> ref_points;
	//template<class Archive>
	//void serialize(Archive& archive)
	//{
	//	archive(height, width, 
	//		divs_x, divs_y, 
	//		grid_interval_x, grid_interval_y,
	//		calibration_mode, ref_points); // serialize things by passing them to the archive
	//}
} GridConfig;

typedef struct {
	bool flip_x;
	bool flip_y;

	float x_offset;
	float y_offset;

	//template<class Archive>
	//void serialize(Archive& archive)
	//{
	//	archive(flip_x, flip_y, x_offset, y_offset); // serialize things by passing them to the archive
	//}
} MeasurementConfig;

typedef struct {
	int paint_mode;
	int erase_radius;
	int paint_density_counter;
	int paint_density_interval;
}PainterConfig;


typedef struct {
	ViewConfig* perspective_view_config;
	ViewConfig* ortho_view_config;
	ApplicationConfig* app_config;
	GridConfig* grid_config;
	MeasurementConfig* measurement_config;
	ImageConfig* img_config;
	PainterConfig* paint_config;
} SessionConfig;