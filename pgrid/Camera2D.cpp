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

#include "Camera2D.h"

Camera2D::Camera2D(ViewConfig* view_config) {
	this->view_config = view_config;
}

void Camera2D::apply_cam() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	const double w = width / ppu;
	const double h = height / ppu;

	glOrtho(-w / 2, w / 2, h / 2, - h / 2, -1, 1);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glScalef(scale, -scale, 1.0);
	glTranslatef(-x, y, 0);
}

void Camera2D::update() {
	x = view_config->pan_x;
	y = view_config->pan_y;
	scale = view_config->zoom;
}


void Camera2D::set_width(double w) {
	width = w;
}

void Camera2D::set_height(double h) {
	height = h;
}

void Camera2D::set_x(double x_pos) {
	x = x_pos;
}

void Camera2D::set_y(double y_pos) {
	y = y_pos;
}

void Camera2D::set_scale(double s) {
	scale = s;
}

glm::dvec2 Camera2D::mouse_to_scene_coords(double u, double v) {
	const double w = width / ppu;
	const double h = height / ppu;
	glm::mat4 projMat = glm::ortho(-w/2, w/2, h/2, -h/2);

	glm::mat4 inverse = glm::inverse(projMat);

	glm::dvec4 mouse_pos = glm::dvec4(u, v, 0, 1);
	glm::dvec4 result = projMat * mouse_pos;
	glm::dvec2 test = glm::dvec2(result.x, result.y);
	//glm::mat4 scaleMat = glm::scale()

	//glm::dvec2 test = glm::dvec2((u * width) / (2 * scale) - (x * width / scale),
	//	(v * height) / (2 * scale) - (y * height / scale));
	//
	//test = glm::dvec2((2 * scale / width) * (u + x * width), (2 * scale / height) * (v + y * height));
	//
	//test.x = test.x * width;
	//test.y = -1*( test.y * height);
	//
	return test;
}