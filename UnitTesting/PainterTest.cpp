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

#include "pch.h"
#include "CppUnitTest.h"
#include "../pgrid/Painter.h"
#include "../pgrid/Config.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(PainterTest) {

	public:

		TEST_METHOD(init_painter) {
			SessionConfig* config = new SessionConfig;

			config->app_config = new ApplicationConfig;
			config->grid_config = new GridConfig;
			config->img_config = new ImageConfig;
			config->measurement_config = new MeasurementConfig;
			config->ortho_view_config = new ViewConfig;
			config->perspective_view_config = new ViewConfig;
			config->paint_config = new PainterConfig;

			Painter painter(config);
		}
		TEST_METHOD(add_point) {

			SessionConfig* config = new SessionConfig;

			config->app_config = new ApplicationConfig;
			config->grid_config = new GridConfig;
			config->img_config = new ImageConfig;
			config->measurement_config = new MeasurementConfig;
			config->ortho_view_config = new ViewConfig;
			config->perspective_view_config = new ViewConfig;
			config->paint_config = new PainterConfig;

			Painter painter(config);

			config->paint_config->paint_density_counter = 0;
			config->paint_config->paint_density_interval = 2;

			painter.add_point(0, 0);
			Assert::IsTrue(config->paint_config->paint_density_counter == 1);
			painter.add_point(0, 0);
			Assert::IsTrue(painter.size() == 0);
			Assert::IsTrue(config->paint_config->paint_density_counter == 2);
			painter.add_point(0, 0);
			Assert::IsTrue(painter.size() == 1);
			Assert::IsTrue(config->paint_config->paint_density_counter == 0);
			painter.add_point(0, 0);
			Assert::IsTrue(config->paint_config->paint_density_counter == 1);
			painter.add_point(0, 0);
			Assert::IsTrue(painter.size() == 1);
			Assert::IsTrue(config->paint_config->paint_density_counter == 2);
			painter.add_point(0, 0);
			Assert::IsTrue(painter.size() == 2);
			Assert::IsTrue(config->paint_config->paint_density_counter == 0);

		}
		TEST_METHOD(add_point_at_click) {
			SessionConfig* config = new SessionConfig;

			config->paint_config = new PainterConfig;

			Painter painter(config);

			config->paint_config->paint_density_counter = 0;
			config->paint_config->paint_density_interval = 2;

			painter.add_point_at_click(0, 0);
			Assert::IsTrue(config->paint_config->paint_density_counter == 0);
			Assert::IsTrue(painter.size() == 1);
			painter.add_point_at_click(0, 0);
			Assert::IsTrue(config->paint_config->paint_density_counter == 0);
			Assert::IsTrue(painter.size() == 2);
		}
		TEST_METHOD(project_points_display) {

		}
		TEST_METHOD(project_points) {

		}

		// TODO move this method to image
		TEST_METHOD(scene_to_uv_coord) {
			SessionConfig* config = new SessionConfig;

			config->img_config = new ImageConfig;
			config->app_config = new ApplicationConfig;

			Painter painter(config);

			config->app_config->image = new Image(config);

			config->app_config->image->get_height();
			config->app_config->image->get_width();
			
			//std::vector<Point2f>
		}
	};
}
