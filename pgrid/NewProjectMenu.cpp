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

#include "NewProjectMenu.h"
NewProjectMenu::NewProjectMenu(SessionConfig* config) {
	app_config = config->app_config;
}

void NewProjectMenu::choose_project_file() {
	nfdchar_t* file_path = NULL;

	nfdresult_t result = NFD_OpenDialog(NULL, NULL, &file_path);

	if (result == NFD_OKAY) {
		strcpy_s(project_file_path, (char*)file_path);

		free(file_path);
	}
	else if (result == NFD_CANCEL) {
		return;
	}
	else {
		MessageBox(NULL, "Fatal error when choosing project save file", "Error!", MB_OK);
		exit(1);
	}

}

void NewProjectMenu::choose_image_files(SeatConfiguration* seat_config) {
	nfdpathset_t* file_paths = NULL;

	nfdresult_t result = NFD_OpenDialogMultiple(NULL, NULL, file_paths);
	
	if (result == NFD_OKAY) {
		for (int i = 0; i < NFD_PathSet_GetCount(file_paths); i++) {
			std::string filepath = (char*)NFD_PathSet_GetPath(file_paths, i);
			seat_config->image_filepaths.push_back(filepath);
		}

		NFD_PathSet_Free(file_paths);
	}
	else if (result == NFD_CANCEL) {
		return;
	}
	else {
		MessageBox(NULL, "Fatal error when choosing image files", "Error!", MB_OK);
		exit(1);
	}

}

void NewProjectMenu::commit_settings() {

}

void NewProjectMenu::add_seat_configuration() {
	SeatConfiguration* seat_config = new SeatConfiguration;

	seat_config->neck_height = 0;
	seat_config->seat_height = 0;
	seat_config->seat_track = 0;
	
	seat_configs.push_back(*seat_config);

}

void NewProjectMenu::add_seat_configuration(std::string neck_height,
	std::string seat_track,
	std::string seat_height,
	std::vector<std::string> image_filepaths) {

}

void NewProjectMenu::layout() {

	const uint16_t u16_one = 1;
	ImGui::InputScalar("Vehicle Year",ImGuiDataType_U16, &vehicle_year, &u16_one, &u16_one, "%04d");
	ImGui::InputText("Vehicle Make", &vehicle_make);
	ImGui::InputText("Vehicle Model", &vehicle_model);
	ImGui::InputText("Description", &description);
	ImGui::InputText("Comments", &comments);
	ImGui::Combo("Vehicle Class", &vehicle_class, vehicle_class_str, IM_ARRAYSIZE(vehicle_class_str));

	ImGui::SeparatorText("Seat Configurations");

	if (ImGui::Button("Add Configuration")) {
		add_seat_configuration();
	}

	for (unsigned int i = 0; i < seat_configs.size(); i++) {
		if (ImGui::TreeNode((void*)(intptr_t)i, "Seat config %d", i)) {
			ImGui::Combo("Neck height", &(seat_configs[i].neck_height), neck_heights, IM_ARRAYSIZE(neck_heights));
			ImGui::Combo("Seat track", &(seat_configs[i].seat_track), seat_tracks, IM_ARRAYSIZE(seat_tracks));
			ImGui::Combo("Seat Height", &(seat_configs[i].seat_height), seat_heights, IM_ARRAYSIZE(seat_heights));

			if (ImGui::Button("Choose Image Files")) {
				choose_image_files(&(seat_configs[i]));
			}

			if (ImGui::Button("Delete")) {
				seat_configs.erase(seat_configs.begin() + i);
			}
			ImGui::TreePop();
		}
	}

	ImGui::Separator();
	if (ImGui::Button("Cancel")) {
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Project")) {

		choose_project_file();
		ImGui::CloseCurrentPopup();
	}
}