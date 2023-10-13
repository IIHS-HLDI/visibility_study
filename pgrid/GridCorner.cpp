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

#include "GridCorner.h"

GridCorner::GridCorner(float x_init, float y_init,
	bool set_origin) {
	x = x_init;
	y = y_init;
	is_origin = set_origin;
}

void GridCorner::make_origin() {
	is_origin = true;
}

void GridCorner::clear_origin() {
	is_origin = false;
}

void GridCorner::draw() {
	glLineWidth(1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	glColor3f(0.0f, 1.0f, 0.0f);

	if (is_origin) {
		glColor3f(0.0f, 0.0f, 1.0f);
	}

	glBegin(GL_POLYGON);
		glVertex2f(x - corner_size, y - corner_size);
		glVertex2f(x - corner_size, y + corner_size);
		glVertex2f(x + corner_size, y + corner_size);
		glVertex2f(x + corner_size, y - corner_size);
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

