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

#include "ReferencePoint.h"

ReferencePoint::ReferencePoint(SessionConfig* session_config, double x, double y, int idx) {
	grid_config = session_config->grid_config;
	this->idx = idx;
	this->x = x;
	this->y = y;
	grid_x = 0;
	grid_y = 0;
	grid_coords[0] = 0;
	grid_coords[1] = 0;
}

void ReferencePoint::draw() {

	//char title[20];
	//sprintf_s(title, 20, "Reference Point %d", idx);
	//ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Once);
	//ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
	//ImGui::Begin(title);
	//
	//ImGui::InputFloat2("Point", &x, &y, "%.3f");
	//ImGui::InputFloat2("Grid Coord", &x, &y, ".3f");

	//ImGui::Text("Added Point at %f, %f", this->x, this->y);

	//ImGui::End();

	//ImGui::Text("Added Point at %f, %f", this->x, this->y);

	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_POLYGON);
		glVertex2f((GLfloat) x - 5, (GLfloat) y - 5);
		glVertex2f((GLfloat) x - 5, (GLfloat) y + 5);
		glVertex2f((GLfloat) x + 5, (GLfloat) y + 5);
		glVertex2f((GLfloat) x + 5, (GLfloat) y - 5);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1.0, 1.0, 1.0);
}

void ReferencePoint::set_grid_coords(int x, int y) {
	grid_x = x;
	grid_y = y;

	ref_x = grid_config->grid_interval_x * grid_x;
	ref_y = grid_config->grid_interval_y * grid_y;
}

int* ReferencePoint::grid_x_ptr() {
	return &grid_x;
}
int* ReferencePoint::grid_y_ptr() {
	return &grid_y;
}

int ReferencePoint::get_grid_x() {
	return grid_y;
}

int ReferencePoint::get_grid_y() {
	return grid_y;
}

double ReferencePoint::get_x() {
	return x;
}
double ReferencePoint::get_y() {
	return y;
}
float ReferencePoint::get_ref_x() {
	return ref_x;
}
float ReferencePoint::get_ref_y() {
	return ref_y;
}

void ReferencePoint::set_world_coords(float x, float y) {
	ref_x = x;
	ref_y = y;
}
