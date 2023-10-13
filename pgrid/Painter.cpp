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


#include "Painter.h"


/**
* Default initializer for painter class
* 
* @param session_config Pointer to SessionConfig struct
*/
Painter::Painter(SessionConfig* session_config) {

	// Copy pointer addresses to Painter object for easy access
	grid_config = session_config->grid_config;
	paint_config = session_config->paint_config;
	img_config = session_config->img_config;
	app_config = session_config->app_config;
	measurement_config = session_config->measurement_config;
}

/**
* Adds a Nearest Visible Point (NVP) to the Painter object. Note that calling add_point does not guarantee the creation of a new point.
* The add_point method limits the number of points using paint_density_counter and paint_density_interval located in the paint_config struct. 
*
* Let N represent the paint_density_interval and i represent paint_density_counter. If i >= N, a new point is pushed. If i < N, i is incremented by one.
* 
* This method has paint density control because it is called on a mouse click and drag event. Without the density control, a new point would be added
* every frame, which gives you many more points than are needed. 
* 
* @param x x coordinate of the requested point in the perspective scene coordinate system
* @param y y coordinate of the requested point in the perspective scene coordinate system
*/
void Painter::add_point(float x, float y) {

	// Check if paint_density_counter has reached paint_density_interval
	if (paint_config->paint_density_counter >= paint_config->paint_density_interval) {

		// Add a new point to the list
		points.push_back(cv::Point2f(x, y));

		// Reproject the points for display code
		project_points_display();

		// reset paint_density_counter
		paint_config->paint_density_counter = 0;
	}
	else {
		// Increment paint_density_counter if the interval has not yet been reached
		paint_config->paint_density_counter++;
	}

}

/**
* Adds a Nearest Visible Point (NVP) to the Painter object. In contrast to the add_point method, add_point_at_click guarantees the creation of a new point. This is because it is called on a mouse click event and does not 
* take into account mouse movement. 
* 
* @param x x coordinate of the new point in the perspective scene coordinate system
* @param y y coordinate of the new point in the perspective scene coordinate system
*/
void Painter::add_point_at_click(float x, float y) {
	points.push_back(cv::Point2f(x, y));
	project_points_display();
}

/**
* Draws point in the perspective panel
*/
void Painter::draw() {

	// Set color to red
	glColor3f(1.0f, 0.0f, 0.0f);

	// Set polygon mode to line so that points display as square outlines
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Loop through all of the points in the painter object
	for (unsigned int i = 0; i < points.size(); i++) {

		// Draw square around the point. View radius is half the side length of drawn square (in scene coordinate system).
		glBegin(GL_POLYGON);
			glVertex2f(points[i].x - view_radius, points[i].y - view_radius);
			glVertex2f(points[i].x - view_radius, points[i].y + view_radius);
			glVertex2f(points[i].x + view_radius, points[i].y + view_radius);
			glVertex2f(points[i].x + view_radius, points[i].y - view_radius);
		glEnd();


		// Draw crosshair that shows the center of the point.
		glBegin(GL_LINES);
			glVertex2f(points[i].x - 2, points[i].y);
			glVertex2f(points[i].x + 2, points[i].y);
		glEnd();

		glBegin(GL_LINES);
			glVertex2f(points[i].x, points[i].y + 2);
			glVertex2f(points[i].x, points[i].y - 2);
		glEnd();
	}
	// Reset draw mode and color
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1.0f, 1.0f, 1.0f);
}

/**
* Draws point in the orthographic panel
*/
void Painter::draw_ortho() {

	// Set color to red
	glColor3f(1.0f, 0.0f, 0.0f);

	// Set polygon mode to line so that points display as square outlines
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Loop through all of the points in the painter object
	for (unsigned int i = 0; i < points.size(); i++) {
		// Draw square around the point. view_radius is half the side length of the drawn square (in scene coordinate system)
		glBegin(GL_POLYGON);
			glVertex2f(projected_points_disp[i].x - view_radius, projected_points_disp[i].y - view_radius);
			glVertex2f(projected_points_disp[i].x - view_radius, projected_points_disp[i].y + view_radius);
			glVertex2f(projected_points_disp[i].x + view_radius, projected_points_disp[i].y + view_radius);
			glVertex2f(projected_points_disp[i].x + view_radius, projected_points_disp[i].y - view_radius);
		glEnd();

		// Draw crosshair that shows the center of the point
		glBegin(GL_LINES);
			glVertex2f(projected_points_disp[i].x - crosshair_size, projected_points_disp[i].y);
			glVertex2f(projected_points_disp[i].x + crosshair_size, projected_points_disp[i].y);
		glEnd();

		glBegin(GL_LINES);
			glVertex2f(projected_points_disp[i].x, projected_points_disp[i].y + crosshair_size);
			glVertex2f(projected_points_disp[i].x, projected_points_disp[i].y - crosshair_size);
		glEnd();
	}
	// Reset draw mode and color
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1.0f, 1.0f, 1.0f);
}

/**
* Project points for display purposes (not the final exported measurements)
*/
void Painter::project_points_display() {

	// Check that there are points to project at all. If not, there is no need to proceed
	if (points.size() > 0) {


		//TODO: check for camera profile
		//std::vector<cv::Point2f> uv_points = scene_to_uv_coord(points);
		//std::vector<cv::Point2f> undistorted_points;
		//if (true) {
		//	undistorted_points = img_config->camera_profile->undistort_points(uv_points);
		//}
		//else {
		//
		//	undistorted_points = uv_coords_to_scene_coords(uv_points);
		//}

		// Resize the projected_points_disp if points has changed size
		projected_points_disp.reserve(points.size()); 

		if (grid_config->calibration_mode == 2 || grid_config->calibration_mode == 1) {// If using Aruco marker calibration or point calibration
			// Applies transformation matrix to points and stores results in projected_points_disp
			//undistorted_points = uv_coords_to_scene_coords(undistorted_points);
			cv::perspectiveTransform(points, projected_points_disp, grid_config->grid->get_inverse_transform());
		}
		else if (grid_config->calibration_mode == 0) {// If using grid corner calibration
			//points = uv_coords_to_scene_coords(points);
			// Applies transformation matrix to points and stores results in projected_points_disp
			cv::perspectiveTransform(points, projected_points_disp, grid_config->grid->get_inverse_transform());

			// Loop through all of the resulting points and apply grid offsets, measurement offsets, and flip_x/flip_y
			for (unsigned int i = 0; i < projected_points_disp.size(); i++) {
				// Check flip_x and flip_y for display
				float flip_x = 1;
				float flip_y = 1;
				if (measurement_config->flip_x) {
					flip_x = -1;
				}
				if (measurement_config->flip_y) {
					flip_y = -1;
				}
				// measurement_config->x_offset and measurement_config->y_offset are multiplied by 100 to match the visual scale of the grid on screen since the offsets
				// are internally stored in meters.
				projected_points_disp[i].x = flip_x*(projected_points_disp[i].x - grid_config->grid->corner[0].x) - measurement_config->x_offset*100;
				projected_points_disp[i].y = flip_y*(projected_points_disp[i].y - grid_config->grid->corner[0].y) - measurement_config->y_offset*100;
			}
		}
		else if (grid_config->calibration_mode == 3) {// If using markerless calibration mode

			// Convert scene points (origin at the center of image) to u v coordinates (origin at the top-left corner of the image)
			std::vector<cv::Point2f> uv_points = scene_to_uv_coord(points);

			// Use the img_to_world_transform defined by the current camera profile to transform uv_points to world points
			projected_points_disp = img_config->camera_profile->img_to_world_transform(uv_points, *img_config->cam_pose);

		}
	}
}

/**
* Project points for final exported measurements
* 
* @return list of projected 2D points in the world coordinate frame (bird's eye view above the vehicle with perspective removed) with units of meters
*/
std::vector<cv::Point2f> Painter::project_points() {
	std::vector<cv::Point2f> projected_points(points.size());

	if (grid_config->calibration_mode == 2 || grid_config->calibration_mode == 1) { // If using point calibration or aruco marker calibration
		// transform points to projected_points using grid inverse transform ([O] matrix)
		cv::perspectiveTransform(points, projected_points, grid_config->grid->get_inverse_transform());

		// Loop through all of the projected points
		for (unsigned int i = 0; i < projected_points.size(); i++) {

			// Points are initially scaled to cm (multiplied by 100) for numerical stability when converting 
			// between scene/image coordinates and world coordinates this scaling has to be taken back out for the final output
			// in meters.
			projected_points[i].x = (projected_points[i].x) / 100;
			projected_points[i].y = (projected_points[i].y) / 100;
		}
	}
	else if (grid_config->calibration_mode == 0) { // If using grid corner calibration

		// transform points to projected_points using grid inverse transform ([O] matrix)
		cv::perspectiveTransform(points, projected_points, grid_config->grid->get_inverse_transform());

		// Loop through all of the projected_points
		for (unsigned int i = 0; i < projected_points.size(); i++) {

			// Points must be referenced from corner0 and divided by 100 to account for the prescaling
			// World points are initially scaled to cm (multiplied by 100) for numerical stability when converting
			// between scene/image coords and world coords. This scaling has to be taken away for the final output
			// in meters.
			projected_points[i].x = (projected_points[i].x - grid_config->grid->corner[0].x) / 100;
			projected_points[i].y = (projected_points[i].y - grid_config->grid->corner[0].y) / 100;
		}
	}
	else if (grid_config->calibration_mode == 3) { // if using markerless calibration

		// Convert scene points (origin at the center of image) to u v coordinates (origin at the top-left corner of the image)
		std::vector<cv::Point2f> uv_points = scene_to_uv_coord(points);

		// project uv_points using loaded camera profile and camera pose
		projected_points = img_config->camera_profile->img_to_world_transform(uv_points, *img_config->cam_pose);

		// Loop through all of the points
		for (unsigned int i = 0; i < projected_points.size(); i++) {
			// World points are initially scaled to cm (multiplied by 100) for numerical stability when converting
			// between scene/image coords and world coords. This scaling has to be taken away for the final output
			// in meters
			projected_points[i].x = projected_points[i].x / 100;
			projected_points[i].y = projected_points[i].y / 100;
		}
	}

	// return projected points
	return projected_points;
}

/**
* 
* Transform points in scene coordinates (origin at center of image) to a list of points in uv coordinates 
* (origin at top-left corner of image)
* 
* @param scene_points vector of points in scene coordinates
* 
* @return List of points transformed to uv coordinates
*/
std::vector<cv::Point2f> Painter::scene_to_uv_coord(std::vector<cv::Point2f> scene_points) {
	// Create vector for transformed points in uv coordinates
	std::vector<cv::Point2f> uv_points(scene_points.size());

	// Loop through vector of scene_points
	for (unsigned int i = 0; i < scene_points.size(); i++) {
		// Transform points based on loaded image
		uv_points[i].x = scene_points[i].x + app_config->image->get_width()/2;
		uv_points[i].y = -1*(scene_points[i].y - app_config->image->get_height()/2);
	}

	// return vector
	return uv_points;
}

/**
* Transform a point in scene coordinates (origin at center of image) to a  point in uv coordinates
* (origin at top-left corner of image)
* 
* @param scene_x x coordinate in scene coordinates
* @param scene_y y coordinate in scene coordinates
* 
* @return vector containing x (0th element of vector) and y (1st element of vector) in uv coords
*/
std::vector<double> Painter::scene_to_uv_coord(double scene_x, double scene_y) {
	// Reserve storage for uv point
	std::vector<double> uv_point(2);

	// Transform points based on loaded image
	uv_point[0] = scene_x + app_config->image->get_width() / 2;
	uv_point[1] = -1*(scene_y - app_config->image->get_height() / 2);

	// return uv_point
	return uv_point;
}

/**
* Transform a point in scene coordinates (origin at center of image) to a  point in uv coordinates
* (origin at top-left corner of image)
*
* @param scene_x x coordinate in scene coordinates
* @param scene_y y coordinate in scene coordinates
*
* @return vector containing x (0th element of vector) and y (1st element of vector) in uv coords
*/
std::vector<double> Painter::uv_coord_to_scene_coord(double u, double v) {
	// Reserve storage for uv point
	std::vector<double> scene_coord(2);

	// Transform points based on loaded image
	scene_coord[0] = u - app_config->image->get_width() / 2;
	scene_coord[1] = -1 * (u + app_config->image->get_height() / 2);

	// return uv_point
	return scene_coord;
}

std::vector<cv::Point2f> Painter::uv_coords_to_scene_coords(std::vector<cv::Point2f> uv_points) {
	// Create vector for transformed points in uv coordinates
	std::vector<cv::Point2f> scene_coords(uv_points.size());

	// Loop through vector of scene_points
	for (unsigned int i = 0; i < uv_points.size(); i++) {
		// Transform points based on loaded image
		scene_coords[i].x = uv_points[i].x - app_config->image->get_width() / 2;
		scene_coords[i].y = -1 * (uv_points[i].y + app_config->image->get_height() / 2);
	}

	// return vector
	return scene_coords;
}

/**
* Computes the distance between points in scene coordinates
* 
* @param x0 x coordinate of point 0
* @param y0 y coordinate of point 0
* @param x1 x coordinate of point 1
* @param y1 y coordinate of point 1
* 
* @return distance as a floating point number
*/
double Painter::distance(double x0, double y0,
	double x1, double y1) {
	return sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
}

/**
* Removes points within a radius of requested x and y coordinate
* 
* @param x x position to erase in scene coordinates
* @param y y position to erase in scene coordinates
*/
void Painter::erase(float x, float y) {

	// Loop through all of the points
	for (unsigned int i = 0; i < points.size(); i++) {
		// check if the points are within the erase radius of requested erase location
		if (distance(x, y, points[i].x, points[i].y) <= paint_config->erase_radius) {
			// if point is in radius remove it from the vector of points
			points.erase(std::remove(points.begin(), points.end(), points[i]), points.end());
		}
	}
}

/**
* Get the number of points
* 
* @return number of points
*/
int Painter::size() {
	return (int) points.size();
}

/**
* Clears all points
* 
*/
void Painter::clear_points() {
	points.clear();
}