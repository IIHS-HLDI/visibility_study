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

#include "PerspectivePanel.h"

/**
* Initializer for PerspectivePanel. Creates panel, creates a grid member at a default location, creates a 2d camera, and reserves space for an image
* 
* @param session_config pointer to shared session config settings 
*/
PerspectivePanel::PerspectivePanel(SessionConfig* session_config) :
	grid(session_config, 
		-100, -100, 
		-100, 100, 
		100, 100, 
		100, -100),
	camera(session_config->perspective_view_config), 
	image(session_config) {
	this->width = 0;
	this->height = 0;
	
	// setup pointers for easy config access
	view_config = session_config->perspective_view_config;
	app_config = session_config->app_config;
	grid_config = session_config->grid_config;

	// set grid pointer in grid_config so that other objects can access it
	grid_config->grid = &grid;

}

/**
* Stores a reference to the main GLFWwindow for access in the PerspectivePanel class
* this should be called by Application's init function after initialization.
*/
void PerspectivePanel::set_window_pointer(GLFWwindow* window) {
	this->window = window;
}

/**
* Initialize dynamically allocated framebuffer, texture, and renderbuffer for opengl to 
* render to. This texture will be displayed in an ImGui window
* 
*/
void PerspectivePanel::init() {
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &tex);
	glGenRenderbuffers(1, &rbo);
}

/**
* Passthrough function to initiate image loading from main application
*/
void PerspectivePanel::load_image() {
	image.load();
}

/**
* Renders everything in the perspective panel to an OpenGL framebuffer, which then gets rendered to an ImGui texture
*/
void PerspectivePanel::render() {

	// Begin Perspective Scene Window
	ImGui::Begin("Perspective Scene");

	// Get the available size in the perspective scene window
	ImVec2 available_size = ImGui::GetWindowContentRegionMax();

	// get window position
	window_pos = ImGui::GetWindowPos();
	
	// set height and width of the window
	this->width = available_size.x;
	this->height = available_size.y;

	// update 2D camera with height and width
	camera.set_height(height);
	camera.set_width(width);
	camera.update();

	// Bind framebuffer and texture
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		(GLsizei) width, (GLsizei) height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei) width, (GLsizei) height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// If there was a problem with the creation of the framebuffer, report the issue
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		MessageBox(NULL, "There was a fatal error when binding an OpenGL framebuffer to PerspectivePanel", "Error!", MB_OK);
		exit(1);
	}

	// Set the viewport size to height and width of the window
	glViewport((GLint) 0, (GLint) 0, (GLsizei) width, (GLsizei) height);

	// Clear the screen
	glClearColor(0.00f, 0.00f, 0.00f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Apply 2d camera transformation
	camera.apply_cam();

	// Render the image to the screen
	image.render();

	// Render the virtual grid to screen
	// for any mode other than markerless
	if (grid_config->calibration_mode != 3) {
		grid.draw();
	}

	// Draw painted nearest visible points (NVPs)
	app_config->painter->draw();

	// bind default framebuffer, i.e. render to main framebuffer instead of 
	// perspective panel framebuffer
	// Everything that is contained within the perspective panel window e.g. the perspective grid, 
	// image, and NVPs should be rendered in the perspective panel framebuffer. That is 
	// they should be rendered before this call to switch back to the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Display the perspective panel texture as an image in ImGui window
	ImGui::Image((ImTextureID)static_cast<uintptr_t>(tex),
		ImGui::GetContentRegionAvail(),
		ImVec2(0, 1),
		ImVec2(1, 0));

	// End perspective panel window
	ImGui::End();
}

/**
* Cleans up OpenGL memory and calls image deconstructor
* 
*/
void PerspectivePanel::close() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &tex);
	glDeleteRenderbuffers(1, &rbo);

	image.close();
}

/**
* Returns the window x position in screen coordinates
* 
* @return x position of the window in screen coordinates
*/
float PerspectivePanel::get_pos_x() {
	return window_pos.x;
}

/**
* Returns the window y position in screen coordinates
* 
* @return y position of the window in screen coordinates
*/
float PerspectivePanel::get_pos_y() {
	return window_pos.y;
}

/**
* Checks if the mouse position passed is within the bounds of the perspective panel window
* This prevents users from inadvertently performing actions on the perspective panel window, when
* the mouse pointer is outside of the perspective panel
* 
* @return true if the mouse is within the bounds of the perspective panel, false otherwise
*/
bool PerspectivePanel::is_mouse_on(double mx, double my) {

	return(window_pos.x <= mx && mx < window_pos.x + width -5 && 
		window_pos.y <= my && my <= window_pos.y + height);
}


/**
* Converts image u v coordinates to scene coordinates for aruco marker calibration and display
* OpenCV uses uv coordinates to define the position of the detected aruco markers, but to draw the corners of detected markers
* where they should be in the perspective panel requires the scene position.
* 
* @param quad_points vector of 4-element, Point2f vectors. Each element in the outer vector represents a detected marker quad and each element
* in the inner vector represents the position of a corner of a marker quad in image coordinates
* 
* @return vector of 4-element, Point2f vectors with each element in the outer vector representing a detected marker quad and each element in the
* inner vector representing the position of a corner of a marker quad in scene coordinates
*/
std::vector <std::vector<cv::Point2f>> PerspectivePanel::image_to_scene_pos(std::vector<std::vector<cv::Point2f>> quad_points) {

	std::vector<std::vector<cv::Point2f>> transformed(quad_points.size());

	for (int j = 0; j < quad_points.size(); j++) {
		transformed[j] = std::vector<cv::Point2f>(4);
		for (int k = 0; k < quad_points[j].size(); k++) {
			transformed[j][k].x = quad_points[j][k].x - (app_config->image->get_width()/2);
			transformed[j][k].y = -quad_points[j][k].y + (app_config->image->get_height()/2);
		}
		//std::cout << quad_points[j][0];
	}

	return transformed;
}

/**
* Convert mouse coordinate to coordinate in the perspective scene (origin at the center scaled to image pixels)
* This is typically called when the user interacts with the perspective panel to get the coordinates to draw a point or grab 
* a grid corner.
* 
* @param mx x position of mouse in screen coordinates (origin in top left of window, x positive to the right)
* @param my y position of mouse in screen coordinates (origin in top left of window, y positive down)
* 
* @return vector with the x and y scene position
*/
glm::dvec2 PerspectivePanel::mouse_to_scene_pos(double mx, double my) {

	glm::dvec2 scene_pos = glm::dvec2(
		((mx - (window_pos.x)) - (width+10) / 2) / view_config->zoom + view_config->pan_x,
		-1 * ((my - (window_pos.y)) - (height+28) / 2) / view_config->zoom - view_config->pan_y);

	return scene_pos;
}