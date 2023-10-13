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

#include "CameraProfile.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define sind(x) (sin(fmod((x),360) * M_PI / 180))
#define cosd(x) (cos(fmod((x),360) * M_PI / 180))
#include <cassert>

CameraProfile::CameraProfile() {

}

int CameraProfile::load_profile(const std::string& input_file) {
    cv::FileStorage infile(input_file, cv::FileStorage::READ);

    infile["device"] >> device;
    infile["profile_descriptor"] >> profile_descriptor;
    infile["camera_matrix"] >> camera_intrinsic;
    infile["dist_coeffs"] >> dist_coeffs;
    infile["zoom_level"] >> zoom_level;
    infile["focal_length_mm"] >> focal_length_mm;
    return 0;
}

void CameraProfile::calibrate(const std::string& input_file_dir, std::vector<int> checkerboard_dims) {
    //cv::setNumThreads(0);
    assert(checkerboard_dims.size() == 2);

    // Creating vector to store vectors of 3D points for each checkerboard image
    std::vector<std::vector<cv::Point3f> > objpoints;

    // Creating vector to store vectors of 2D points for each checkerboard image
    std::vector<std::vector<cv::Point2f> > imgpoints;

    // Defining the world coordinates for 3D points
    std::vector<cv::Point3f> objp;
    for (int i{ 0 }; i < checkerboard_dims[1]; i++)
    {
        for (int j{ 0 }; j < checkerboard_dims[0]; j++)
            objp.push_back(cv::Point3f(j, i, 0));
    }


    // Extracting path of individual image stored in a given directory
    std::vector<cv::String> images;
    // Path of the folder containing checkerboard images
    cv::glob(input_file_dir + "\\*.jpg", images);

    cv::Mat frame, gray;
    // vector to store the pixel coordinates of detected checker board corners 
    std::vector<cv::Point2f> corner_pts;
    bool success;
    cv::namedWindow("image", cv::WINDOW_NORMAL);
    // Looping over all the images in the directory
    for (int i{ 0 }; i < images.size(); i++)
    {
        std::cout << "Image " << i << std::endl;
        std::cout << images[i] << std::endl;

        frame = cv::imread(images[i]);
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Finding checker board corners
        // If desired number of corners are found in the image then success = true  
        success = cv::findChessboardCorners(
            gray, 
            cv::Size(checkerboard_dims[0], checkerboard_dims[1]), 
            corner_pts, 
            cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
        //success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

        /*
         * If desired number of corner are detected,
         * we refine the pixel coordinates and display
         * them on the images of checker board
        */
        if (success)
        {
            cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);

            // refining pixel coordinates for given 2d points.
            cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

            // Displaying the detected corner points on the checker board
            cv::drawChessboardCorners(frame, cv::Size(checkerboard_dims[0], checkerboard_dims[1]), corner_pts, success);

            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }

        cv::imshow("image", frame);
        cv::waitKey(0);
    }


    cv::Mat camera_intrinsic, dist_coeffs, R, T;

    /*
     * Performing camera calibration by
     * passing the value of known 3D points (objpoints)
     * and corresponding pixel coordinates of the
     * detected corners (imgpoints)
    */
    cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows, gray.cols), camera_intrinsic, dist_coeffs, R, T);

    //sstd::cout << "cameraMatrix : " << cameraMatrix << std::endl;
    //sstd::cout << "distCoeffs : " << distCoeffs << std::endl;
    //sstd::cout << "Rotation vector : " << R << std::endl;
    //sstd::cout << "Translation vector : " << T << std::endl;

   //e cv::FileStorage K("C:\\pixel_6_pro.mat", cv::FileStorage::WRITE);
   //e K << "camera_matrix" << cameraMatrix;
   //e K << "dist_coeffs" << distCoeffs;

    cv::destroyAllWindows();
}

void CameraProfile::calibrate(const std::string& input_file_dir, int* checkerboard_dims) {
    //cv::setNumThreads(0);
    //assert(checkerboard_dims.size() == 2);

    // Creating vector to store vectors of 3D points for each checkerboard image
    std::vector<std::vector<cv::Point3f> > objpoints;

    // Creating vector to store vectors of 2D points for each checkerboard image
    std::vector<std::vector<cv::Point2f> > imgpoints;

    // Defining the world coordinates for 3D points
    std::vector<cv::Point3f> objp;
    for (int i{ 0 }; i < checkerboard_dims[1]; i++)
    {
        for (int j{ 0 }; j < checkerboard_dims[0]; j++)
            objp.push_back(cv::Point3f(j, i, 0));
    }


    // Extracting path of individual image stored in a given directory
    std::vector<cv::String> images;
    // Path of the folder containing checkerboard images
    cv::glob(input_file_dir + "\\*.jpg", images);

    cv::Mat frame, gray;
    // vector to store the pixel coordinates of detected checker board corners 
    std::vector<cv::Point2f> corner_pts;
    bool success;
    cv::namedWindow("image", cv::WINDOW_NORMAL);
    // Looping over all the images in the directory
    for (int i{ 0 }; i < images.size(); i++)
    {
        std::cout << "Image " << i << std::endl;
        std::cout << images[i] << std::endl;

        frame = cv::imread(images[i]);
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Finding checker board corners
        // If desired number of corners are found in the image then success = true  
        success = cv::findChessboardCorners(
            gray,
            cv::Size(checkerboard_dims[0], checkerboard_dims[1]),
            corner_pts,
            cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
        //success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

        /*
         * If desired number of corner are detected,
         * we refine the pixel coordinates and display
         * them on the images of checker board
        */
        if (success)
        {
            cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);

            // refining pixel coordinates for given 2d points.
            cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

            // Displaying the detected corner points on the checker board
            cv::drawChessboardCorners(frame, cv::Size(checkerboard_dims[0], checkerboard_dims[1]), corner_pts, success);

            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }

        cv::imshow("image", frame);
        cv::waitKey(0);
    }


    cv::Mat R, T;

    /*
     * Performing camera calibration by
     * passing the value of known 3D points (objpoints)
     * and corresponding pixel coordinates of the
     * detected corners (imgpoints)
    */
    cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows, gray.cols), camera_intrinsic, dist_coeffs, R, T);

    //sstd::cout << "cameraMatrix : " << cameraMatrix << std::endl;
    //sstd::cout << "distCoeffs : " << distCoeffs << std::endl;
    //sstd::cout << "Rotation vector : " << R << std::endl;
    //sstd::cout << "Translation vector : " << T << std::endl;

   //e cv::FileStorage K("C:\\pixel_6_pro.mat", cv::FileStorage::WRITE);
   //e K << "camera_matrix" << cameraMatrix;
   //e K << "dist_coeffs" << distCoeffs;

    cv::destroyAllWindows();
}

void CameraProfile::write_profile(const std::string& output_file) {
    cv::FileStorage outfile(output_file, cv::FileStorage::WRITE);

    outfile << "device" << device;
    outfile << "profile_descriptor" << profile_descriptor;
    outfile << "camera_matrix" << camera_intrinsic;
    outfile << "dist_coeffs" << dist_coeffs;
    outfile << "zoom_level" << zoom_level;
    outfile << "focal_length_mm" << focal_length_mm;
}

void CameraProfile::undistort(cv::Mat src_img, cv::Mat dst_img) {
	cv::undistort(src_img, dst_img, camera_intrinsic, dist_coeffs);
}

std::vector<cv::Point2f> CameraProfile::img_to_world_transform(std::vector<cv::Point2f> img_points, CameraPose camera_pose) {
	cv::Mat cam_translation = (cv::Mat_<double>(3, 1) << camera_pose.x_pos, camera_pose.y_pos, camera_pose.z_pos);
	cv::Mat cam_rotation = (cv::Mat_<double>(3,3) << 1., 0., 0.,
		0., 0., -1.,
		0., 1., 0.);

    cv::Mat yaw_matrix = cv::getRotationMatrix2D(cv::Point2f(camera_pose.x_pos, camera_pose.y_pos), camera_pose.yaw_angle, 1.0);

	//cv::Mat yaw_matrix = (cv::Mat_<double>(2, 2) << 
	//	cosd(camera_pose.yaw_angle), 
	//	-sind(camera_pose.yaw_angle), 
	//	sind(camera_pose.yaw_angle), 
	//	cosd(camera_pose.yaw_angle));


	std::vector<cv::Point2f> world_points(img_points.size());
	for (int i = 0; i < img_points.size(); i++) {

		cv::Mat img_pixel = (cv::Mat_<double>(3, 1) << img_points[i].x, img_points[i].y, 1);

		cv::Mat r = (camera_intrinsic.inv() * img_pixel);

		cv::Mat r_scaled = r * (focal_length_mm / 10.f);

		cv::Mat world_coord = cam_rotation * r_scaled + cam_translation;

		// Intersect parametrically defined line with plane z = 0
		// s: parameter variable of line

		float s = (cam_translation.at<double>(2, 0)) / (world_coord.at<double>(2, 0) - cam_translation.at<double>(2, 0));
		
        cv::Mat intercept = (cv::Mat_<double>(3,1) <<
			(world_coord.at<double>(0,0) - cam_translation.at<double>(0,0))*s + cam_translation.at<double>(0,0), 
			-1*(world_coord.at<double>(1,0) - cam_translation.at<double>(1,0)) * s + cam_translation.at<double>(1,0), 1.0);
        
        cv::Mat ground_point = yaw_matrix * intercept;
		//cv::Mat intercept_mat = cv::Mat(intercept, false);
        //
		//intercept_mat = yaw_matrix * intercept_mat;
		//
		//intercept_mat.copyTo(cv::Mat(intercept, false));

		world_points[i].x = ground_point.at<double>(0,0);
        world_points[i].y = ground_point.at<double>(1, 0);

	}

	return world_points;
}

std::vector<cv::Point2f> CameraProfile::undistort_points(std::vector<cv::Point2f> src_points) {

    std::vector<cv::Point2f> undistorted(src_points.size());

    cv::undistortPoints(src_points, undistorted, camera_intrinsic, dist_coeffs);

    return undistorted;
}


float* CameraProfile::get_focal_length_ptr() {
    return &focal_length_mm;
}

float* CameraProfile::get_zoom_level_ptr() {
    return &zoom_level;
}
std::string* CameraProfile::get_device_ptr() {
    return &device;
}
std::string* CameraProfile::get_profile_descriptor_ptr() {
    return &profile_descriptor;
}

cv::Mat CameraProfile::get_camera_matrix() {
    return camera_intrinsic;
}

cv::Mat CameraProfile::get_dist_coeffs() {
    return dist_coeffs;
}