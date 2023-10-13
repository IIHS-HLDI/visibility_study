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

#include "Grid.h"

Grid::Grid(SessionConfig* session_config, float x0, float y0,
	float x1, float y1,
	float x2, float y2,
	float x3, float y3) {
	
	this->session_config = session_config;
	this->grid_config = session_config->grid_config;
	this->img_config = session_config->img_config;
	this->app_config = session_config->app_config;

	//(this->corner).reserve(4);

	corner.push_back(GridCorner(x0, y0, true));
	corner.push_back(GridCorner(x1, y1, false));
	corner.push_back(GridCorner(x2, y2, false));
	corner.push_back(GridCorner(x3, y3, false));
	//corner[0] = GridCorner(x0, y0, true);
	//corner[1] = GridCorner(x1, y1, false);
	//corner[2] = GridCorner(x2, y2, false);
	//corner[3] = GridCorner(x3, y3, false);
}

void Grid::compute_perspective_transform() {

	if(grid_config->calibration_mode == 0) {
		std::vector<cv::Point2f> src_shape(4);
		std::vector<cv::Point2f> dst_shape(4);

		src_shape[0] = cv::Point2f(corner[0].x, corner[0].y);
		src_shape[1] = cv::Point2f(corner[1].x, corner[1].y);
		src_shape[2] = cv::Point2f(corner[2].x, corner[2].y);
		src_shape[3] = cv::Point2f(corner[3].x, corner[3].y);

		dst_shape[0] = cv::Point2f(corner[0].x, corner[0].y);
		dst_shape[1] = cv::Point2f(corner[0].x, corner[0].y + (grid_config->height)*100);
		dst_shape[2] = cv::Point2f(corner[0].x + (grid_config->width)*100, corner[0].y + (grid_config->height) * 100);
		dst_shape[3] = cv::Point2f(corner[0].x + (grid_config->width)*100, corner[0].y);

		this->pM = cv::getPerspectiveTransform(dst_shape, src_shape);
		this->pM_inv = cv::getPerspectiveTransform(src_shape, dst_shape);
		return;
	}
	else if (grid_config->calibration_mode == 1) {
		int rp_size = grid_config->ref_points.size();
		std::vector<cv::Point2f> src_shape(rp_size);
		std::vector<cv::Point2f> dst_shape(rp_size);

		for (unsigned int i = 0; i < rp_size; i++) {
			src_shape[i].x = grid_config->ref_points[i].get_x();
			src_shape[i].y = grid_config->ref_points[i].get_y();
			
			dst_shape[i].x = grid_config->ref_points[i].get_ref_x() * 100;
			dst_shape[i].y = grid_config->ref_points[i].get_ref_y()* 100;
			//src_shape[i] = cv::Point2f(grid_config->ref_points[i].get_x(), grid_config->ref_points[i].get_y());
			//dst_shape[i] = cv::Point2f(grid_config->ref_points[i].get_ref_x(), grid_config->ref_points[i].get_ref_y());
		}


		this->pM = cv::findHomography(dst_shape, src_shape,
			cv::RHO,
			8, cv::noArray(), 5000);
		this->pM_inv = cv::findHomography(src_shape, dst_shape,
			cv::RHO,
			8, cv::noArray(), 5000);

		std::vector<cv::Point2f> ortho_corners(4);
		ortho_corners[0].x = 0;
		ortho_corners[0].y = 0;
		ortho_corners[1].x = 0;
		ortho_corners[1].y = (grid_config->height)*100;
		ortho_corners[2].x = (grid_config->width)*100;
		ortho_corners[2].y = (grid_config->height)*100;
		ortho_corners[3].x = (grid_config->width)*100;
		ortho_corners[3].y = 0;

		std::vector < cv::Point2f> persp_corners(4);

		cv::perspectiveTransform(ortho_corners, persp_corners, pM);
		corner[0].x = persp_corners[0].x;
		corner[0].y = persp_corners[0].y;
		corner[1].x = persp_corners[1].x;
		corner[1].y = persp_corners[1].y;
		corner[2].x = persp_corners[2].x;
		corner[2].y = persp_corners[2].y;
		corner[3].x = persp_corners[3].x;
		corner[3].y = persp_corners[3].y;
				
		//this->pM = cv::getPerspectiveTransform(dst_shape, src_shape);
		//this->pM_inv = cv::getPerspectiveTransform(src_shape, dst_shape);
		return;
	}
	else if (grid_config->calibration_mode == 2) {
		//this->pM = cv::getPerspectiveTransform(img_config->world_points, img_config->scene_points);
		//this->pM_inv = cv::getPerspectiveTransform(img_config->scene_points, img_config->world_points);

		std::vector<cv::Point2f> scaled_world_points(img_config->world_points.size());


		for (int i = 0; i < scaled_world_points.size(); i++) {
			scaled_world_points[i].x = img_config->world_points[i].x * 100;
			scaled_world_points[i].y = img_config->world_points[i].y * 100;
		}

		this->pM = cv::findHomography(scaled_world_points, img_config->scene_points, cv::RHO, 8, cv::noArray(), 5000);
		this->pM_inv = cv::findHomography(img_config->scene_points, scaled_world_points, cv::RHO, 8, cv::noArray(), 5000);

		std::vector<cv::Point2f> ortho_corners(4);
		ortho_corners[0].x = 0;
		ortho_corners[0].y = 0;
		ortho_corners[1].x = 0;
		ortho_corners[1].y = (grid_config->height)*100;
		ortho_corners[2].x = (grid_config->width)*100;
		ortho_corners[2].y = (grid_config->height)*100;
		ortho_corners[3].x = (grid_config->width)*100;
		ortho_corners[3].y = 0;
		
		std::vector < cv::Point2f> persp_corners(4);
		
		cv::perspectiveTransform(ortho_corners, persp_corners, pM);
		
		corner[0].x = persp_corners[0].x;
		corner[0].y = persp_corners[0].y;
		corner[1].x = persp_corners[1].x;
		corner[1].y = persp_corners[1].y;
		corner[2].x = persp_corners[2].x;
		corner[2].y = persp_corners[2].y;
		corner[3].x = persp_corners[3].x;
		corner[3].y = persp_corners[3].y;

		return;
	}
	else if (grid_config->calibration_mode == 3) {
		// do nothing in markerless mode
	}
	
}

void Grid::draw_ortho() {

	std::vector<cv::Point2f> src_shape(4);
	std::vector<cv::Point2f> dst_shape(4);

	src_shape[0] = cv::Point2f(corner[0].x, corner[0].y);
	src_shape[1] = cv::Point2f(corner[1].x, corner[1].y);
	src_shape[2] = cv::Point2f(corner[2].x, corner[2].y);
	src_shape[3] = cv::Point2f(corner[3].x, corner[3].y);

	dst_shape[0] = cv::Point2f(0, 0);
	dst_shape[1] = cv::Point2f(0, grid_config->height*100);
	dst_shape[2] = cv::Point2f( grid_config->width*100,  grid_config->height*100);
	dst_shape[3] = cv::Point2f(grid_config->width*100, 0);

	std::vector<cv::Point2f> ortho_corners(4);

	cv::perspectiveTransform(src_shape, ortho_corners, pM_inv);
	glLineWidth(1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0.0f, 0.0f, 1.0f);

	for (unsigned int i = 0; i < ortho_corners.size(); i++) {
		glBegin(GL_POLYGON);
		glVertex2f(dst_shape[i].x - 50, dst_shape[i].y - 50);
		glVertex2f(dst_shape[i].x - 50, dst_shape[i].y + 50);
		glVertex2f(dst_shape[i].x + 50, dst_shape[i].y + 50);
		glVertex2f(dst_shape[i].x + 50, dst_shape[i].y - 50);
		glEnd();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	float x_offset = (grid_config->width * 100) / grid_config->divs_x;
	for (int i = 1; i < grid_config->divs_x; i++) {
		std::vector<cv::Point2f> points(2);
		std::vector<cv::Point2f> ortho(2);
		points[0] = cv::Point2f(dst_shape[0].x + x_offset * i, dst_shape[0].y);
		points[1] = cv::Point2f(dst_shape[1].x + x_offset * i, dst_shape[1].y);

		cv::perspectiveTransform(points, ortho, pM);

		glColor3f(1.00f, 0.38f, 0.00f);
		if ((grid_config->divs_x - i + 3) % 5 == 0) {
			glColor3f(0.00f, 1.00f, 1.00f);
		}

		//glBegin(GL_LINES);
		//glVertex2f(ortho[0].x, ortho[0].y);
		//glVertex2f(ortho[1].x, ortho[1].y);
		//glEnd();
		glBegin(GL_LINES);
		glVertex2f(points[0].x, points[0].y);
		glVertex2f(points[1].x, points[1].y);
		glEnd();
	}

	float y_offset = (grid_config->height * 100) / grid_config->divs_y;

	for (int i = 1; i < grid_config->divs_y; i++) {
		std::vector<cv::Point2f> points(2);
		std::vector<cv::Point2f> ortho(2);

		points[0] = cv::Point2f(dst_shape[0].x, dst_shape[0].y + y_offset * i);
		points[1] = cv::Point2f(dst_shape[3].x, dst_shape[3].y + y_offset * i);

		cv::perspectiveTransform(points, ortho, pM);

		glColor3f(1.00f, 0.38f, 0.00f);
		if ((grid_config->divs_y - i + 0) % 5 == 0) {
			glColor3f(1.00f, 0.00f, 1.00f);
		}

		//glBegin(GL_LINES);
		//glVertex2f(ortho[0].x, ortho[0].y);
		//glVertex2f(ortho[1].x, ortho[1].y);
		//glEnd();
		glBegin(GL_LINES);
		glVertex2f(points[0].x, points[0].y);
		glVertex2f(points[1].x, points[1].y);
		glEnd();
	}
}

void Grid::draw() {
	for (int i = 0; i < 4; i++) {
		corner[i].draw();
	}

	// TODO: make line colors a configurable option
	glColor3f(1.00f, 0.38f, 0.00f);

	glBegin(GL_LINES);
		glVertex2f(corner[0].x, corner[0].y);
		glVertex2f(corner[1].x, corner[1].y);
	glEnd();
	glBegin(GL_LINES);
		glVertex2f(corner[1].x, corner[1].y);
		glVertex2f(corner[2].x, corner[2].y);
	glEnd();
	glBegin(GL_LINES);
		glVertex2f(corner[2].x, corner[2].y);
		glVertex2f(corner[3].x, corner[3].y);
	glEnd();
	glBegin(GL_LINES);
		glVertex2f(corner[3].x, corner[3].y);
		glVertex2f(corner[0].x, corner[0].y);
	glEnd();

	std::vector<cv::Point2f> src_shape(4);
	std::vector<cv::Point2f> dst_shape(4);
	
	src_shape[0] = cv::Point2f(corner[0].x, corner[0].y);
	src_shape[1] = cv::Point2f(corner[1].x, corner[1].y);
	src_shape[2] = cv::Point2f(corner[2].x, corner[2].y);
	src_shape[3] = cv::Point2f(corner[3].x, corner[3].y);
	
	dst_shape[0] = cv::Point2f(corner[0].x, corner[0].y);
	dst_shape[1] = cv::Point2f(corner[0].x, corner[0].y + grid_config->height);
	dst_shape[2] = cv::Point2f(corner[0].x + grid_config->width, corner[0].y + grid_config->height);
	dst_shape[3] = cv::Point2f(corner[0].x + grid_config->width, corner[0].y);

	//pM = cv::getPerspectiveTransform(dst_shape, src_shape);
	//pM_inv = cv::getPerspectiveTransform(src_shape, dst_shape);

	//std::vector<cv::Point2f> perspective_corners(4);
	if (grid_config->calibration_mode == 1) {
		//static int prev_ref_point_size = grid_config->ref_points.size();
		compute_perspective_transform();
	}
	else {
		compute_perspective_transform();
	}

	std::vector<cv::Point2f> ortho_corners(4);

	cv::perspectiveTransform(src_shape, ortho_corners, pM_inv);
	glLineWidth(1);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glColor3f(0.0f, 0.0f, 1.0f);
	//
	//for (unsigned int i = 0; i < ortho_corners.size(); i++) {
	//	glBegin(GL_POLYGON);
	//		glVertex2f(ortho_corners[i].x - 50, ortho_corners[i].y - 50);
	//		glVertex2f(ortho_corners[i].x - 50, ortho_corners[i].y + 50);
	//		glVertex2f(ortho_corners[i].x + 50, ortho_corners[i].y + 50);
	//		glVertex2f(ortho_corners[i].x + 50, ortho_corners[i].y - 50);
	//	glEnd();
	//}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	float x_offset = (grid_config->width * 100) / grid_config->divs_x;
	for(int i = 1; i < grid_config->divs_x; i++) {
		std::vector<cv::Point2f> points(2);
		std::vector<cv::Point2f> ortho(2);
		points[0] = cv::Point2f(ortho_corners[0].x + x_offset * i, ortho_corners[0].y);
		points[1] = cv::Point2f(ortho_corners[1].x + x_offset * i, ortho_corners[1].y);

		cv::perspectiveTransform(points, ortho, pM);

		glColor3f(1.00f, 0.38f, 0.00f);
		if ((grid_config->divs_x - i + 3) % 5 == 0) {
			glColor3f(0.00f, 1.00f, 1.00f);
		}

		glBegin(GL_LINES);
			glVertex2f(ortho[0].x, ortho[0].y);
			glVertex2f(ortho[1].x, ortho[1].y);
		glEnd();
		//glBegin(GL_LINES);
		//	glVertex2f(points[0].x, points[0].y);
		//	glVertex2f(points[1].x, points[1].y);
		//glEnd();
	}

	float y_offset = (grid_config->height* 100) / grid_config->divs_y;

	for (int i = 1; i < grid_config->divs_y; i++) {
		std::vector<cv::Point2f> points(2);
		std::vector<cv::Point2f> ortho(2);

		points[0] = cv::Point2f(ortho_corners[0].x, ortho_corners[0].y + y_offset * i);
		points[1] = cv::Point2f(ortho_corners[3].x, ortho_corners[3].y + y_offset * i);

		cv::perspectiveTransform(points, ortho, pM);

		glColor3f(1.00f, 0.38f, 0.00f);
		if ((grid_config->divs_y - i + 0) % 5 == 0) {
			glColor3f(1.00f, 0.00f, 1.00f);
		}

		glBegin(GL_LINES);
			glVertex2f(ortho[0].x, ortho[0].y);
			glVertex2f(ortho[1].x, ortho[1].y);
		glEnd();
		//glBegin(GL_LINES);
		//	glVertex2f(points[0].x, points[0].y);
		//	glVertex2f(points[1].x, points[1].y);
		//glEnd();
	}  
	for (unsigned int i = 0; i < grid_config->ref_points.size(); i++) {
		grid_config->ref_points[i].draw();
	}

	glColor3f(1.0f, 1.0f, 1.0f);
}

float Grid::distance(float x0, float y0,
	float x1, float y1) {
	return sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
}

int Grid::grab(float x, float y) {
	static int click_radius = 100;
	if (distance(x, y, corner[0].x, corner[0].y) <= click_radius) {
		return 0;
	}
	else if (distance(x, y, corner[1].x, corner[1].y) <= click_radius) {
		return 1;
	}
	else if (distance(x, y, corner[2].x, corner[2].y) <= click_radius) {
		return 2;
	}
	else if (distance(x, y, corner[3].x, corner[3].y) <= click_radius) {
		return 3;
	}
	else {
		return -1;
	}
}


void Grid::move_corner(int corner_idx,
	float x, float y) {
	if (!(corner_idx >= 0 && corner_idx < 4)) {
		std::cout << "Error: invalid corner index" << std::endl;
	}
	corner[corner_idx].x = x;
	corner[corner_idx].y = y;
}

void Grid::nudge_corner(int corner_idx,
	float x, float y) {
	corner[corner_idx].x += x;
	corner[corner_idx].y += y;
}

void Grid::add_ref_point(double mx, double my) {
	//ref_points.push_back(cv::Point2f((float)mx, (float)my));
	//world_points.push_back(cv::Point2f(0, 0));
	
	//ImGui::Text("%f, %f", mx, my);
	grid_config->ref_points.push_back(ReferencePoint(session_config, mx, my, grid_config->ref_points.size()));
	compute_perspective_transform();
}

cv::Mat Grid::get_perspective_transform() {
	return pM;
}

cv::Mat Grid::get_inverse_transform() {
	return pM_inv;
}