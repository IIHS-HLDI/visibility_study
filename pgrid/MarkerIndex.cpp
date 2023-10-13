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

#include "MarkerIndex.h"

MarkerIndex::MarkerIndex(SessionConfig* session_config) {
	app_config = session_config->app_config;

	build_index();
}

cv::Point2f MarkerIndex::lookup(unsigned int id) {
	return cv::Point2f(x_coords[id], y_coords[id]);
}

void MarkerIndex::build_index() {
	std::ifstream index_file;

	index_file.open(app_config->marker_index_filepath);

	if (!index_file.is_open()) {
		exit(30);
	}

	int id;
	float x, y;
	std::string ignore;
	while (index_file >> ignore >> id >> ignore >> x >> ignore >> y) {
		marker_ids.push_back(id);
		x_coords.push_back(x);
		y_coords.push_back(y);
	}
}