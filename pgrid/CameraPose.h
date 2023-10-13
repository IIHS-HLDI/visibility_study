#pragma once

typedef struct {
	// Position of camera wrt world coordinate frame
	double x_pos;
	double y_pos;
	double z_pos;

	// Rotation of camera
	double pitch_angle; // should be zero for most images unless camera was tilted towards the ground or the sky 
						// avoid changing pitch angle if at all possible

	double yaw_angle; // z rotation in world coordinate frame. e.g. an image with angle 0 is frontal view
					  // image towards drivers side is + 90
					  // image towards passengers side is -90
}CameraPose;