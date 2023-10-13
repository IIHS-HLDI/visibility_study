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

#include "ControlPanel.h"
#include "Config.h"
#include "Painter.h"
#include "Image.h"
#include "PerspectivePanel.h"
#include "nfd.h"


ControlPanel::ControlPanel(SessionConfig* config) {
	//session_config = config;
	app_config = config->app_config;
	view_config = config->perspective_view_config;
	grid_config = config->grid_config;
	measurement_config = config->measurement_config;
	img_config = config->img_config;
	paint_config = config->paint_config;
}

void ControlPanel::choose_output_file() {
	nfdchar_t* outpath = NULL;

	nfdresult_t result = NFD_SaveDialog(NULL, NULL, &outpath);

	if (result == NFD_OKAY) {
		puts("Success!");
		puts(outpath);

		wchar_t** path = NULL;

		strcpy_s(app_config->outfile_path, (char*)outpath);
		free(outpath);

		free(path);
	}
	else if (result == NFD_CANCEL) {
		puts("User pressed cancel.");
	}
	else {
		printf("Error: %s\n", NFD_GetError());
	}
}

void ControlPanel::choose_calibration_dir() {
	nfdchar_t* cal_dir = NULL;

	nfdresult_t result = NFD_PickFolder("C:\\", &cal_dir);

	if (result == NFD_OKAY) {
		puts("Success!");
		puts(cal_dir);

		wchar_t** path = NULL;

		img_config->camera_profile->calibrate(cal_dir, { 12,9 });

		//strcpy_s(app_config->outfile_path, (char*)outpath);
		//free(outpath);
		//
		//free(path);
	}
	else if (result == NFD_CANCEL) {
		puts("User pressed cancel.");
	}
	else {
		printf("Error: %s\n", NFD_GetError());
	}
}

void ControlPanel::choose_camera_profile() {
	nfdchar_t* file_path = NULL;

	nfdresult_t result = NFD_OpenDialog(NULL, NULL, &file_path);

	if (result == NFD_OKAY) {
		puts("Success!");
		puts(file_path);

		img_config->camera_profile->load_profile(file_path);

		//img_config->camera_profile->calibrate(cal_dir, { 12,9 });

		//strcpy_s(app_config->outfile_path, (char*)outpath);
		free(file_path);
		//
		//free(path);
	}
	else if (result == NFD_CANCEL) {
		puts("User pressed cancel.");
	}
	else {
		printf("Error: %s\n", NFD_GetError());
	}
}

void ControlPanel::choose_image_file() {
	nfdchar_t* outpath = NULL;

	nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outpath);

	if (result == NFD_OKAY) {
		puts("Success!");
		puts(outpath);

		wchar_t** path = NULL;

		strcpy_s(img_config->image_filepath, (char*)outpath);
		free(outpath);

		//app_config->perspective_panel->load_image();
		//perspective_panel.load_image();
		free(path);
	}
	else if (result == NFD_CANCEL) {
		puts("User pressed cancel.");
	}
	else {
		printf("Error: %s\n", NFD_GetError());
	}
}

void ControlPanel::layout() {
	ImGui::Begin("Control Menu");
	ImGui::SetWindowFontScale(1.0);

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Camera Profile")) {
		if (img_config->camera_profile) {
			char* cstr_profile_desc = new char[img_config->camera_profile->profile_descriptor.length() + 1];
			std::strcpy(cstr_profile_desc, img_config->camera_profile->profile_descriptor.c_str());
			ImGui::Text("Loaded profile : %s", cstr_profile_desc);
			if (ImGui::Button("Load camera profile")) {
				choose_camera_profile();
			}

			if (ImGui::Button("Calibrate a new camera")) {
				choose_calibration_dir();
			}
		}
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Import Image")) {
		ImGui::InputText("Image filepath", img_config->image_filepath, 256); 
		ImGui::SameLine();
		if (ImGui::Button("...")) {
			choose_image_file();
		}

		if (ImGui::Button("Load Image")) {
			app_config->perspective_panel->load_image();
		}
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Grid Alignment")) {
		ImGui::Combo("Alignment Mode", &(grid_config->calibration_mode),
			grid_calibration_modes, IM_ARRAYSIZE(grid_calibration_modes));

		if (ImGui::Button("Find Markers")) {
			app_config->image->find_markers();
		}
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Camera Pose")) {

		ImGui::SeparatorText("Camera Position");
		ImGui::InputDouble("X Position (cm)", &(img_config->cam_pose->x_pos), 1.0, 1.0, "%.3f");
		ImGui::InputDouble("Y Position (cm)", &(img_config->cam_pose->y_pos), 1.0, 1.0, "%.3f");
		ImGui::InputDouble("Z Position (cm)", &(img_config->cam_pose->z_pos), 1.0, 1.0, "%.3f");

		ImGui::SeparatorText("Camera Rotation");
		ImGui::InputDouble("Yaw Angle (deg)", &(img_config->cam_pose->yaw_angle), 1.0, 1.0, "%.3f");
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Tool Mode")) {
		ImGui::Text("Select Mode");

		ImGui::RadioButton("Grid", &app_config->mode, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Point Align", &app_config->mode, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Paint", &app_config->mode, 2);
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Paint Settings")) {
		ImGui::SeparatorText("Select Tool");

		ImGui::RadioButton("Paintbrush", &paint_config->paint_mode, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Eraser", &paint_config->paint_mode, 1);

		if (ImGui::Button("Clear Points")) {
			ImGui::OpenPopup("Clear Points?");
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Clear Points?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Are you sure you want to clear all points?\nThis operation cannot be undone.");
			ImGui::Separator();
			if (ImGui::Button("Yes", ImVec2(120, 0))) {
				app_config->painter->clear_points();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		//Paint Density
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Viewport")) {
		ImGui::SliderFloat("Pan X", &(view_config->pan_x), -1000.0f, 1000.0f);
		ImGui::SliderFloat("Pan Y", &(view_config->pan_y), -1000.0f, 1000.0f);
		ImGui::InputDouble("Zoom", &view_config->zoom, 0.0, 50.0f);

		ImGui::Text("Mouse X: %f, Mouse Y: %f", (view_config->mouse_x), (view_config->mouse_y));
		ImGui::Text("Scene X: %f, Scene Y: %f", (view_config->scene_x), (view_config->scene_y));
		if (img_config->image_loaded) {
			std::vector<double> uv_coord = app_config->painter->scene_to_uv_coord(view_config->scene_x, view_config->scene_y);
			assert(uv_coord.size() == 2);

			ImGui::Text("Image U: %f, Image V: %f", uv_coord[0], uv_coord[1]);
		}
		//World x and world y;
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Corner Controls")) {
		ImGui::SliderFloat("Control Sensitivity", &(app_config->corner_control_sensitivity), 0.0, 10.0f);
		ImGui::Button("1", ImVec2(50, 50));
		ImVec2 mouse_delta = ImGui::GetMouseDragDelta(0, 0.0f);
		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::IsItemActive()) {
			ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
			grid_config->grid->nudge_corner(1,
				mouse_delta.x * app_config->corner_control_sensitivity / app_config->corner_control_scaler,
				-mouse_delta.y * app_config->corner_control_sensitivity / app_config->corner_control_scaler);
		}

		ImGui::SameLine();
		ImGui::Button("2", ImVec2(50, 50));
		if (ImGui::IsItemActive()) {
			ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
			grid_config->grid->nudge_corner(2,
				mouse_delta.x * app_config->corner_control_sensitivity / app_config->corner_control_scaler,
				-mouse_delta.y * app_config->corner_control_sensitivity / app_config->corner_control_scaler);
		}

		ImGui::Button("0", ImVec2(50, 50));
		if (ImGui::IsItemActive()) {
			ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
			grid_config->grid->nudge_corner(0,
				mouse_delta.x * app_config->corner_control_sensitivity / app_config->corner_control_scaler,
				-mouse_delta.y * app_config->corner_control_sensitivity / app_config->corner_control_scaler);
		}

		ImGui::SameLine();
		ImGui::Button("3", ImVec2(50, 50));
		if (ImGui::IsItemActive()) {
			ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
			grid_config->grid->nudge_corner(3,
				mouse_delta.x * app_config->corner_control_sensitivity / app_config->corner_control_scaler,
				-mouse_delta.y * app_config->corner_control_sensitivity / app_config->corner_control_scaler);
		}
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Grid Settings")) {
		ImGui::InputFloat("Grid width (meters)", &(grid_config->width), 0, 100);
		ImGui::InputFloat("Grid height (meters)", &(grid_config->height), 0, 100);
		ImGui::InputInt("Longitudinal divisions", &(grid_config->divs_x), 1, 100);
		ImGui::InputInt("Lateral divisions", &(grid_config->divs_y), 1, 100);
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Reference Points")) {
		for (int i = 0; i < grid_config->ref_points.size(); i++) {
			ImGui::SeparatorText("Reference Point");

			ReferencePoint* ref_point = &(grid_config->ref_points[i]);
			ImGui::PushID(ref_point);
			//ImGui::InputInt2("grid coords", grid_config->ref_points[i].grid_coords);

			ImGui::InputFloat("X coord", &(ref_point->ref_x));
			ImGui::InputFloat("Y coord", &(ref_point->ref_y));

			if (ImGui::Button("Delete Point")) {
				grid_config->ref_points.erase(grid_config->ref_points.begin() + i);
			}

			//grid_config->ref_points[i].set_world_coords(x, y);
			//ImGui::InputInt("X grid coordinate", grid_config->ref_points[i].grid_x_ptr());
			//ImGui::InputInt("Y grid coordinate", grid_config->ref_points[i].grid_y_ptr());

			ImGui::PopID();
			ImGui::Separator();
		}
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Measurement offsets")) {
		ImGui::Checkbox("Flip x", &(measurement_config->flip_x));
		ImGui::Checkbox("Flip y", &(measurement_config->flip_y));
		ImGui::InputFloat("Origin x-offset", &(measurement_config->x_offset), 0.0f, 0.0f, "%.3f");
		ImGui::InputFloat("Origin y-offset", &(measurement_config->y_offset), 0.0f, 0.0f, "%.3f");
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Output Settings")) {
		ImGui::InputText("Output Filepath", app_config->outfile_path, 256); ImGui::SameLine();
		if (ImGui::Button("...")) {
			choose_output_file();
		}
		ImGui::InputText("Image Description", app_config->outfile->get_img_description_buf(), 50);
		ImGui::Text("Image last 4: %04d", app_config->outfile->get_img_last4());
		ImGui::Combo("Neck", app_config->outfile->get_neck_ptr(),
			app_config->outfile->neck_options_display, IM_ARRAYSIZE(app_config->outfile->neck_options_display));
		ImGui::Combo("Seat Track", app_config->outfile->get_seat_track_ptr(),
			app_config->outfile->seat_track_options_display, IM_ARRAYSIZE(app_config->outfile->seat_track_options_display));
		ImGui::Combo("Seat Height", app_config->outfile->get_seat_height_ptr(),
			app_config->outfile->seat_height_options_display, IM_ARRAYSIZE(app_config->outfile->seat_height_options_display));
		ImGui::Checkbox("Append?", app_config->outfile->get_append_ptr());

		if (ImGui::Button("Write Output")) {
			int status = app_config->outfile->open();
			// TODO: logic for edge cases

			app_config->outfile->write_output(app_config->painter->project_points());
			app_config->outfile->close();
		}
		if (app_config->outfile->is_saved()) {
			ImGui::Text("Output has been saved");
		}
		else {
			ImGui::Text("Warning: you have unsaved data");
		}
	}
	ImGui::End();


}