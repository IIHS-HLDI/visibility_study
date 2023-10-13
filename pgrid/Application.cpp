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

#include <shlobj.h>
#include <shlwapi.h>
#include "Application.h"
#include "ControlPanel.h"
#include "OutputFile.h"
#include "MarkerIndex.h"

/**
 * Initializer for Application class. Creates child windows: control panel, perspective panel, orthographic panel, calibration menu (hidden), and new project menu (hidden)
 *
 * @param session_config pointer to shared SessionConfig structure
 */
Application::Application(SessionConfig* session_config)  :
	ctrl_panel(session_config), 
	perspective_panel(session_config),
	ortho_panel(session_config), 
	calibration_menu(session_config),
	new_project_menu(session_config){


    // Assign sub-configurations for easy access
	app_config = session_config->app_config;
	view_config = session_config->perspective_view_config;
	ortho_config = session_config->ortho_view_config;
	grid_config = session_config->grid_config;
	img_config = session_config->img_config;
	paint_config = session_config->paint_config;

    // Create pointers to child classes
    //
    // Painter class manages the nearest visible point (NVP) paint
    // input from the user
    app_config->painter = new Painter(session_config);

    // OutputFile class manages the data output to a CSV file
    app_config->outfile = new OutputFile(session_config);

    // MarkerIndex class is only for Aruco marker calibration. It
    // reads the marker_index database file from the disk into program
    // memory so that aruco marker IDs can be associated with world
    // coordinates
    app_config->marker_index = new MarkerIndex(session_config);

    // Add a reference to perspective panel in app_config so it
    // can be accessed by other objects
    app_config->perspective_panel = &perspective_panel;

    // set default width and height of the application window
	width = 1600;
	height = 1000;


    // set error callback function
	glfwSetErrorCallback(glfw_error_callback);

    // Check if glfw initialized properly
	if (!glfwInit()) {
        MessageBox(NULL, 
                "Fatal error during glfw initialization" , 
                "Error!", MB_OK);
        exit(1);
	}

    // add GLFW context window hints, not sure if or why this is
    // necessary, but it seems to be present in the GLFW examples
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // setup window label this is in an sprintf_s so that in the
    // future other things can be added based on context
    //
    // TODO: add image name/ vehicle name to window label
    sprintf_s(window_label, "%s", "OGRE");

    // Create glfw window
	window = glfwCreateWindow(width, height,
		window_label, NULL, NULL);

    // Check if glfw window initialized properly
	if (window == NULL) {
        MessageBox(NULL, 
                "Fatal error during glfw initialization" , 
                "Error!", MB_OK);
        exit(1);
		/* printf("Window is null\n"); */
	}

    // Embed a reference to this Application object in glfw window
    // object so that application members can be accessed when
    // processing user events. Event callbacks in GLFW have a certain
    // parameter signature, so without this event callback functions
    // won't have access to any members of the application.
    // Thankfully, the glfwwindow class has space for a user defined
    // void pointer that can be dereferenced to access members in the
    // Application object.
    //
    glfwSetWindowUserPointer(window, (void*)this);


    // Set up glfw event callbacks
    //
    // window resize
	glfwSetWindowSizeCallback(window, resize_callback);

    // Cursor position callback. This is for click and drag events
	glfwSetCursorPosCallback(window, cursor_position_callback);
    // Mouse button callback just for click events
	glfwSetMouseButtonCallback(window, mouse_button_callback);
    // Scroll callback for zoom in/out functionality
	glfwSetScrollCallback(window, scroll_callback);
    // Keyboard callback
    // TODO: add keyboard shortcuts
	glfwSetKeyCallback(window, keypress_callback);


	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

    // Check glfw context
	GLenum err = glewInit();
	if (GLEW_OK != err) {
        MessageBox(NULL, 
                "Fatal error during glfw context initialization" , 
                "Error!", MB_OK);
        exit(1);

		/* std::cerr << "Error: " << glewGetErrorString(err) << std::endl; */
		this->close();
		exit(1);
	}
	
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, width, height, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);

	perspective_panel.set_window_pointer(window);
	perspective_panel.init();

	ortho_panel.set_window_pointer(window);
	ortho_panel.init();


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.31f, 0.68f, 1.00f);


	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	main_viewport = ImGui::GetMainViewport();
}

/**
 * Gets the default output directory for the application - the user's Downloads folder in Windows
 */
std::string Application::get_output_directory()
{
	std::string path;
	PWSTR pPath = NULL;
	if (SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_DEFAULT, NULL, &pPath) == S_OK)
	{
		int wlen = lstrlenW(pPath);
		int len = WideCharToMultiByte(CP_ACP, 0, pPath, wlen, NULL, 0, NULL, NULL);
		if (len > 0)
		{
			path.resize(len+1);
			WideCharToMultiByte(CP_ACP, 0, pPath, wlen, &path[0], len, NULL, NULL);
			path[len] = '\\';
		}
		CoTaskMemFree(pPath);
	}
	return path;
}


/**
 * Sets application window width
 * 
 * @param new_width new width of window in pixels
 */
void Application::set_width(uint32_t new_width) {
	width = new_width;
}

/**
 * Sets application window height
 *
 * @param new_height new height of window in pixels
 */
void Application::set_height(uint32_t new_height) {
	height = new_height;
}

/** 
 * Gets called if glfw runs into some error. Stops the program and
 * displays an error message box
 *
 * @param error glfw error number
 * @param description description of the error
 *
 */
void Application::glfw_error_callback(
        int error, 
        const char* description) { 
    /* char error_msg[100] = */ 
    // TODO: add error msg box
    MessageBox(NULL, "Fatal glfw error" , "Error!", MB_OK);
    /* close(); */
    exit(1);
}

void Application::resize_callback(GLFWwindow* window, int new_width, int new_height) {

	Application app = *(Application*)glfwGetWindowUserPointer(window);

	glViewport(0, 0,
		new_width,
		new_height);

	app.set_width(new_width);
	app.set_height(new_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, new_width, new_height, 0.0, 0.0, 1.0f);

	glMatrixMode(GL_MODELVIEW);
}

void Application::cursor_position_callback(GLFWwindow* window, double mx, double my) {
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	// TODO: maybe this belongs in app-config? haven't decided.
	app->view_config->mouse_x = mx;
	app->view_config->mouse_y = my;

	glm::dvec2 scene_pos = app->perspective_panel.mouse_to_scene_pos(mx, my);

	app->view_config->scene_x = scene_pos.x;
	app->view_config->scene_y = scene_pos.y;

	double dx = mx - app->view_config->last_mouse_x;
	double dy = my - app->view_config->last_mouse_y;
	const float speed = 1.0f;
	if (app->perspective_panel.is_mouse_on(mx, my)) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			app->view_config->pan_x -= ((float)dx * (float)speed) / (app->view_config->zoom);
			app->view_config->pan_y -= ((float)dy * (float)speed) / (app->view_config->zoom);
		}
	}

	if (app->ortho_panel.is_mouse_on(mx, my)) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			app->ortho_config->pan_x -= ((float)dx * (float)speed) / (app->ortho_config->zoom);
			app->ortho_config->pan_y -= ((float)dy * (float)speed) / (app->ortho_config->zoom);
		}
	}

	if (app->app_config->mode == 2) {
		if (app->perspective_panel.is_mouse_on(mx, my)) {
			if (app->paint_config->paint_mode == 1) {
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
					app->app_config->painter->erase(app->view_config->scene_x, app->view_config->scene_y);
				}
			}
			else if (app->paint_config->paint_mode == 0) {
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
					app->app_config->painter->add_point(app->view_config->scene_x, app->view_config->scene_y);
					//app->view_config->pan_x -= ((float)dx * (float)speed) / (app->view_config->zoom);
					//app->view_config->pan_y -= ((float)dy * (float)speed) / (app->view_config->zoom);
				}
			}
		}
	}
	else if (app->app_config->mode == 1) {
		if (app->perspective_panel.is_mouse_on(mx, my)) {
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

				int corner = app->grid_config->grid->grab(app->view_config->scene_x, app->view_config->scene_y);

				if (corner >= 0) {
					app->grid_config->grid->move_corner(corner, app->view_config->scene_x, app->view_config->scene_y);
				}
			}
		}
	}

	
	//if ()
	//if (glfwGetMouseButton());

	app->view_config->last_mouse_x = mx;
	app->view_config->last_mouse_y = my;
}
void Application::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	double mx, my;

	glfwGetCursorPos(window, &mx, &my);

	glm::dvec2 scene_pos = app->perspective_panel.mouse_to_scene_pos(mx, my);

	//ImGui::Text("%f, %f", scene_pos.x, scene_pos.y);

	if (app->app_config->mode == 0) {
		if (app->perspective_panel.is_mouse_on(mx, my)) {
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				app->grid_config->grid->add_ref_point(app->view_config->scene_x, app->view_config->scene_y);
				//app->view_config->pan_x -= ((float)dx * (float)speed) / (app->view_config->zoom);
				//app->view_config->pan_y -= ((float)dy * (float)speed) / (app->view_config->zoom);
			}
		}
	}
	else if (app->app_config->mode == 2) {
		if (app->perspective_panel.is_mouse_on(mx, my)) {
			app->app_config->painter->add_point_at_click(scene_pos[0], scene_pos[1]);
		}
	}
	//if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		//popup_menu();
}

void Application::scroll_callback(GLFWwindow* window, double delta_x, double delta_y) {
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	double xpos, ypos;

	glfwGetCursorPos(window, &xpos, &ypos);

	if (app->perspective_panel.is_mouse_on(xpos, ypos)) {
		if (app->view_config->zoom >= 0) {
			app->view_config->zoom += (float)delta_y * (float)app->view_config->zoom_speed;
		}
		else {
			app->view_config->zoom = 0.001;
		}
	}
	if (app->ortho_panel.is_mouse_on(xpos, ypos)) {
		if (app->ortho_config->zoom >= 0) {
			app->ortho_config->zoom += (float)delta_y * (float)app->ortho_config->zoom_speed;
		}
		else {
			app->ortho_config->zoom = 0.001;
		}
	}
	
}

void Application::keypress_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Application app = *(Application*)glfwGetWindowUserPointer(window);
}

bool Application::init() {

	return 1;

}

void Application::import_image() {
	nfdchar_t* outpath = NULL;

	nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outpath);

	if (result == NFD_OKAY) {
		puts("Success!");
		puts(outpath);

		wchar_t** path = NULL;

		strcpy_s(img_config->image_filepath, (char*) outpath);
		free(outpath);

		perspective_panel.load_image();
		free(path);
	}
	else if (result == NFD_CANCEL) {
		puts("User pressed cancel.");
	}
	else {
		printf("Error: %s\n", NFD_GetError());
	}
}

void Application::load_camera_profile() {
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

void Application::main_loop() {
	while (!glfwWindowShouldClose(window)) 
	{

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// set to true to view ImGui demo
		bool show_demo_window = false;
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		main_viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(main_viewport->WorkPos);
		ImGui::SetNextWindowSize(main_viewport->WorkSize);
		ImGui::SetNextWindowViewport(main_viewport->ID);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoCloseButton;
		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
			window_flags |= ImGuiWindowFlags_NoBackground;
		}

		static bool main_window_open = true;
		ImGui::Begin("Main Window", &main_window_open, window_flags | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(1.0f);

		// reset layout to default if no config exists
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			static auto first_time = true;
			if (first_time)
			{
				first_time = false;
				// Clear out existing layout
				ImGui::DockBuilderRemoveNode(dockspace_id);
				// Add empty node
				ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
				// Main node should cover entire window
				ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
				// get id of main dock space area
				ImGuiID dockspace_main_id = dockspace_id;
				// Create a dock node for the right docked window
				ImGuiID right = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Right, 0.35f, nullptr, &dockspace_main_id);

				ImGui::DockBuilderDockWindow("Perspective Scene", dockspace_main_id);
				ImGui::DockBuilderDockWindow("Orthographic Scene", dockspace_main_id);
				ImGui::DockBuilderDockWindow("Control Menu", right);
				ImGui::DockBuilderFinish(dockspace_id);
			}
		}

		std::string menu_action = "";
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New project")) {
					menu_action = "new_project";
				}
				if(ImGui::MenuItem("Load session")) {}

				if (ImGui::BeginMenu("Recent sessions")) {
					ImGui::MenuItem("recent_session1.pgd");
					ImGui::MenuItem("recent2.pgd");
					ImGui::EndMenu();
				}
				if(ImGui::MenuItem("Import image")){
					this->import_image();
				}

				if (ImGui::BeginMenu("Import recent images")) {
					ImGui::MenuItem("file1.jpg");
					ImGui::MenuItem("file2.jpg");
					ImGui::MenuItem("file3.jpg");
					ImGui::EndMenu();
				}

				if(ImGui::MenuItem("Save session")) {}

				if(ImGui::MenuItem("Export data as CSV")) {}

				if(ImGui::MenuItem("Exit Program")) {
					this->close();
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				if(ImGui::MenuItem("Undo")) {}
				if(ImGui::MenuItem("Redo")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools")) {

				if (ImGui::BeginMenu("Camera profile")) {
					if (ImGui::MenuItem("Load existing camera profile")) {
						this->load_camera_profile();
					}
					if (ImGui::MenuItem("New camera profile")) {
						menu_action = "new_cam_profile";
					}

					
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				if(ImGui::MenuItem("Reset viewport")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (menu_action == "new_cam_profile") {
			ImGui::OpenPopup("New camera profile");

		}
		else if (menu_action == "new_project") {
			ImGui::OpenPopup("New Project");
		}


		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("New camera profile", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			calibration_menu.layout();

			ImGui::EndPopup();
		}

		//center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			new_project_menu.layout();
			ImGui::EndPopup();
		}

		ImGuiID dockspace_id = ImGui::GetID("main_dockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), docknode_flags);

		ctrl_panel.layout();

		perspective_panel.render();
		ortho_panel.render();

		ImGui::End();
		ImGui::Render();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);	
	}

    this->close();
}

void Application::close() {

	ortho_panel.close();
	perspective_panel.close();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(0);

}
