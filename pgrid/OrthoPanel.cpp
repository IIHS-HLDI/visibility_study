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

#include "OrthoPanel.h"

OrthoPanel::OrthoPanel(SessionConfig* session_config): camera(session_config->ortho_view_config){
	this->width = 0;
	this->height = 0;

	view_config = session_config->ortho_view_config;
	app_config = session_config->app_config;
	grid_config = session_config->grid_config;
}

void OrthoPanel::set_window_pointer(GLFWwindow* window) {
	this->window = window;
}

void OrthoPanel::init() {
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &tex);
	glGenRenderbuffers(1, &rbo);
}

void OrthoPanel::render() {
	ImGui::Begin("Orthographic Scene");
	ImVec2 available_size = ImGui::GetWindowContentRegionMax();

	window_pos = ImGui::GetWindowPos();
	this->width = available_size.x;
	this->height = available_size.y;

	camera.set_height(height);
	camera.set_width(width);
	camera.update();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuff complete" << std::endl;
	}
	else {
		std::cout << "nobuff" << std::endl;
	}

	glViewport(0.0, 0.0, width, height);

	glClearColor(0.00f, 0.00f, 0.00f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (app_config->painter->size() > 0) {
		app_config->painter->project_points_display();
	}

	camera.apply_cam();

	grid_config->grid->draw_ortho();
	app_config->painter->draw_ortho();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGui::Image((ImTextureID)tex,
		ImGui::GetContentRegionAvail(),
		ImVec2(0, 1),
		ImVec2(1, 0));
	ImGui::End();
}

bool OrthoPanel::is_mouse_on(double mx, double my) {

	return(window_pos.x <= mx && mx < window_pos.x + width - 5 &&
		window_pos.y <= my && my <= window_pos.y + height);
}


void OrthoPanel::close() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &tex);
	glDeleteRenderbuffers(1, &rbo);
}

