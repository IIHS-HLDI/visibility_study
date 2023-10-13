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

#include "CalibrationMenu.h"
#include "nfd.h"

CalibrationMenu::CalibrationMenu(SessionConfig* config) {
	checkerboard_dims[0] = 0;
	checkerboard_dims[1] = 0;
}

void CalibrationMenu::choose_calibration_dir() {
	nfdchar_t* file_path = NULL;

	nfdresult_t result = NFD_PickFolder(NULL, &file_path);

	if (result == NFD_OKAY) {
		puts("Success!");
		puts(file_path);

		//wchar_t** path = NULL;

		//img_config->camera_profile->load_profile(file_path);

		//img_config->camera_profile->calibrate(cal_dir, { 12,9 });

		strcpy_s(cal_img_dir, file_path);
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

void CalibrationMenu::choose_save_file() {
	nfdchar_t* save_path = NULL;

	nfdresult_t result = NFD_SaveDialog(NULL, NULL, &save_path);

	if (result == NFD_OKAY) {
		puts("Success!");
		puts(save_path);


		strcpy_s(save_file_path, (char*)save_path);
		free(save_path);

	}
	else if (result == NFD_CANCEL) {
		puts("User pressed cancel.");
	}
	else {
		printf("Error: %s\n", NFD_GetError());
	}
}

void CalibrationMenu::layout() {
	
	//ImGui::Text("You are creating a new camera profile");
	ImGui::SeparatorText("Camera Details");
	ImGui::InputText("Device", camera_profile.get_device_ptr());
	ImGui::InputText("Profile Descriptor", camera_profile.get_profile_descriptor_ptr());

	ImGui::InputFloat("Camera Zoom Level", camera_profile.get_zoom_level_ptr());
	ImGui::InputFloat("Focal length (mm)", camera_profile.get_focal_length_ptr(), 1.0, 1.0, "%.3f");

	ImGui::SeparatorText("Calibration Setup");

	ImGui::InputText("Calibration Image Directory", cal_img_dir, 100);
	if (ImGui::Button("Select Directory")) {
		choose_calibration_dir();
	}

	ImGui::InputInt2("Checkerboard Dimensions (smaller, larger)", checkerboard_dims);

	if (ImGui::Button("Run Calibration")) {
		camera_profile.calibrate(cal_img_dir, checkerboard_dims);
	}

	ImGui::InputText("Camera Profile Save File", save_file_path, 100); ImGui::SameLine();
	if (ImGui::Button("Select File")) {
		this->choose_save_file();
	}

	if (ImGui::Button("Cancel")) {
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Profile")) {
		camera_profile.write_profile(save_file_path);
		ImGui::CloseCurrentPopup();
	}
}