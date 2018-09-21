#pragma once

struct gunPlanExemplar {
	bulletPlanExemplar bulletInfo;
	bullet bulletBody;
	int startTime = -1;
	int endTime = -1, laserPreparingEndTime = -1;
	char angleType = 'a';
	float gunEndAngle = 0, angleSpeed = 0, gunAngle = 0, laserShootAngle = 0, spawnOffsetAngle = 0;
	sf::Vector2f startCoords, endMovingCoords, gunSpeed;
	std::string commandType;
	bool isRotateClockwise = false;
	float laserSize = 0;
	//'a' for absolute
	//'r' for relative without gunAngle
	//'s' for relative with gunAngle
	char startMovingType = 'a';
};
struct zonePlanExemplar {
	sf::Vector2f UpLeft, DownRight;
	int startBeat, endBeat;
	std::string type;
};
struct camPlanExemplar {
	std::string type = "";
	int b_start_time = -1, b_end_time = -1, b_start_animation = -1, b_end_animation = -1;
	float cam_center_x = 0, cam_center_y = -1, end_angle = 0, flashlight_size = 0, blinkAlpha = 0;
	float zoom = 1;
	bool is_rotate_direction_clockwise = false, is_player_center = false, is_flashlight_active = false;

};
struct BPMchangeExemplar {
	float offset, bpm;
};
