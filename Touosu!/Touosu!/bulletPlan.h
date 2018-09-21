#pragma once
struct bulletPlanExemplar {

	//a - absolute music time
	//r - relative music time (from spawn time to action time)
	//w - after touching with wall
	std::vector<char> timeType;
	std::vector<int> startTime;
	//"abs" for absolute angle of speed direction (need speed, directional angle)
	//"rel" for angle relative from gun angle or prev bullet direction (need speed, directional angle)
	//"plr" for angle to player from gun (with agnle offset and player coord offset) (need speed, angle offset, coord offset)
	//"splr" for angle to player from spawn pos (with agnle offset and player coord offset)
	//"coord" for angle to any coordinate from gun
	//"scoord" for angle to any coordinate from spawn pos
	std::vector<std::string> speedAngleType;
	//"abs" for acceleration from any direction (directional angle)
	//"rel" for acceleration from bullet speed direction (with angle offset)
	//"plr" for acceleration to player coords (with angle offset and player coord offset)
	//"coord" for acceleration to any coordinate
	//"scoord" like coord, but update every step
	//"srel" like rel, but update every step
	//"splr" like plr, but update every step
	//"sabs" like abs, but use gun angle as OY
	std::vector<std::string> accelAngleType;

	//'a' for absolute speed
	//'r' for relative speed
	std::vector<char> speedChangeType;

	std::vector<sf::Vector2f> speedOffsetCoord, accelOffsetCoord;
	std::vector<float> lineBulletSpeed, lineBulletAccel, bulletSize, bulletSpeedAngle, bulletAccelAngle;

	//1-16 color ID, 0 for random
	std::vector<int> bulletColor;

	//wave, circle, standart, ellipse, small_knife, crystal, rune, bullet, dark_ellipse, small_star, alpha_standart, small_dark, 
	//small_ellipse, disk, glowing, bunny_shit, big_star, big_standart, butterfly, flower, big_knife, big_ellipse, very_big, heart, arrow, small_glowing
	//burning, wtf, very_big_standart, note
	std::vector<std::string> bulletSkin;

	//'b' - bounce from walls
	//'d' - destroy by walls
	//'i' - ingnore walls
	//'t' - teleport to opposive wall
	//'c' - classic teleport to opposive wall relative center
	std::vector<char> bulletActionWithWalls;

};