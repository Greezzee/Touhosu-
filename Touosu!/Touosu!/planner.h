#pragma once
#include <fstream>
using namespace std;

struct bulletPlanExemplar {

	//a - absolute music time
	//r - relative music time (from spawn time to action time)
	//w - after touching with wall
	vector<char> timeType;
	vector<int> startTime;
	//"abs" for absolute angle of speed direction (need speed, directional angle)
	//"rel" for angle relative from gun angle or prev bullet direction (need speed, directional angle)
	//"plr" for angle to player from gun (with agnle offset and player coord offset) (need speed, angle offset, coord offset)
	//"splr" for angle to player from spawn pos (with agnle offset and player coord offset)
	//"coord" for angle to any coordinate from gun
	//"scoord" for angle to any coordinate from spawn pos
	//"rand" for rangom angle (in rangle from 0 to angle)
	vector<string> speedAngleType;
	//"abs" for acceleration from any direction (directional angle)
	//"rel" for acceleration from bullet speed direction (with angle offset)
	//"plr" for acceleration to player coords (with angle offset and player coord offset)
	//"coord" for acceleration to any coordinate
	//"scoord" like coord, but update every step
	//"srel" like rel, but update every step
	//"splr" like plr, but update every step
	//"sabs" like abs, but use gun angle as OY
	//"rand" for rangom angle (in range from 0 to angle)
	vector<string> accelAngleType;

	//'a' for absolute speed
	//'r' for relative speed
	vector<char> speedChangeType;

	vector<sf::Vector2f> speedOffsetCoord, accelOffsetCoord;
	vector<float> lineBulletSpeed, lineBulletAccel, bulletSize, bulletSpeedAngle, bulletAccelAngle;

	//1-16 color ID, 0 for random
	vector<int> bulletColor;

	//wave, circle, standart, ellipse, small_knife, crystal, rune, bullet, dark_ellipse, small_star, alpha_standart, small_dark, 
	//small_ellipse, disk, glowing, bunny_shit, big_star, big_standart, butterfly, flower, big_knife, big_ellipse, very_big, heart, arrow, small_glowing
	//burning, wtf, very_big_standart, note
	vector<string> bulletSkin;

	//'b' - bounce from walls
	//'d' - destroy by walls
	//'i' - ingnore walls
	//'t' - teleport to opposive wall
	//'c' - classic teleport to opposive wall relative center
	vector<char> bulletActionWithWalls;

};
struct gunPlanExemplar {
	bulletPlanExemplar bulletInfo;
	int startTime = -1;
	int endTime = -1, laserPreparingEndTime = -1;
	char angleType = 'a';
	float gunEndAngle = 0, angleSpeed = 0, shootAngle = 0;
	sf::Vector2f startMovingCoords, endMovingCoords, gunSpeed;
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
	string type;
};
struct camPlanExemplar {
	string type = "";
	int b_start_time = -1, b_end_time = -1, b_start_animation = -1, b_end_animation = -1;
	float cam_center_x = 0, cam_center_y = -1, end_angle = 0, flashlight_size = 0, blinkAlpha = 0;
	float zoom = 1;
	bool is_rotate_direction_clockwise = false, is_player_center = false, is_flashlight_active = false;

};
struct BPMchangeExemplar {
	float offset, bpm;
};

class planner {
public:
	void init() {
		gunPlanList.resize(0);
		zonePlanList.resize(0);
		camPlanList.resize(0);
		publicInfo.resize(0);
		currentGunStep.resize(0);
		currentCamStep = 0;
		numberOfGuns = 0;
		ifstream file;
		file.open("plan.txt");
		file >> numberOfGuns >> randomSeed;
		gunPlanList.resize(numberOfGuns);
		currentGunStep.resize(numberOfGuns);
		readFile(&file);
		for (unsigned int i = 0; i < gunPlanList.size(); i++)
			sort(gunPlanList[i].begin(), gunPlanList[i].end(), [](const gunPlanExemplar& plan1, const gunPlanExemplar& plan2) -> bool {
				return plan1.startTime < plan2.startTime;
			});
		file.close();
	};

	gunPlanExemplar getGunPlan(int gunID) {
		currentGunStep[gunID]++;
		if (gunPlanList[gunID].size() >= currentGunStep[gunID]) return gunPlanList[gunID][currentGunStep[gunID] - 1];
		else return gunPlanList[gunID][0];
	}
	camPlanExemplar getCamPlan() {
		currentCamStep++;
		if (camPlanList.size() >= currentCamStep) return camPlanList[currentCamStep - 1];
		else return camPlanList[0];
	}
	vector<zonePlanExemplar> getZonePlan() {
		return zonePlanList;
	}
	vector<BPMchangeExemplar> getBPMchangesPlan() {
		return BPMchangePlanList;
	}
	int getRandomSeed() {
		return randomSeed;
	}
	int getNumberOfGuns() {
		return numberOfGuns;
	}
private:
	vector<vector<gunPlanExemplar>> gunPlanList;
	vector<zonePlanExemplar> zonePlanList;
	vector<camPlanExemplar> camPlanList;
	vector<gunPlanExemplar> publicInfo;
	vector<BPMchangeExemplar> BPMchangePlanList;
	vector<unsigned int> currentGunStep;
	unsigned int currentCamStep;
	unsigned int numberOfGuns, randomSeed;

	void readFile(ifstream *file) {
		string command_type;
		do {
			command_type = readOneCommand(file);
		} while (command_type != "end");
	}

	string readOneCommand(ifstream *file) {
		string trash, command_type;
		*file >> command_type;
		if (command_type == "g") {
			int id;
			*file >> id;
			gunPlanExemplar new_plan = createNewPlanExemplar(file);
			gunPlanList[id].push_back(new_plan);
		}
		else if (command_type == "cam") {
			camPlanExemplar plan = readCam(file);
			camPlanList.push_back(plan);
		}
		else if (command_type == "zone") {
			zonePlanExemplar plan = readZone(file);
			zonePlanList.push_back(plan);
		}
		else if (command_type == "new_bpm") BPMchangePlanList.push_back(readBPMchange(file));
		
		else if (command_type == "public");
		else if (command_type == "new_pb") setNewPublicBullet(file);
		else if (command_type == "for") {
			int numberOfActions;
			string trash;
			*file >> numberOfActions >> trash;
			int deltaTime = read_time(file);
			*file >> trash;
			int id;
			*file >> id;
			gunPlanExemplar new_plan = createNewPlanExemplar(file);
			
			for (int i = 0; i < numberOfActions; i++) {
				gunPlanList[id].push_back(new_plan);
				new_plan.startTime += deltaTime;
				new_plan.endTime += deltaTime;
				new_plan.laserPreparingEndTime += deltaTime;
			}
		}
		return command_type;
	}

	gunPlanExemplar createNewPlanExemplar(ifstream *file) {
		gunPlanExemplar new_plan;
		string trash;
		*file >> trash;
		new_plan.startTime = read_time(file);
		*file >> trash;
		*file >> new_plan.commandType;
		if (new_plan.commandType == "set") new_plan = readSet(file, new_plan);
		else if (new_plan.commandType == "del");
		else if (new_plan.commandType == "move") new_plan = readMove(file, new_plan);
		else if (new_plan.commandType == "rotate") new_plan = readRotate(file, new_plan);
		else if (new_plan.commandType == "laser_shoot") new_plan = readLaserShoot(file, new_plan);
		else if (new_plan.commandType == "bullet_shoot") new_plan = readBulletShoot(file, new_plan);
		return new_plan;
	}

	gunPlanExemplar readSet(ifstream *file, gunPlanExemplar new_plan) {
		*file >> new_plan.endMovingCoords.x >> new_plan.endMovingCoords.y >> new_plan.shootAngle;
		return new_plan;
	}
	gunPlanExemplar readMove(ifstream *file, gunPlanExemplar new_plan) {
		string trash;
		*file >> trash;
		new_plan.endTime = read_time(file);
		*file >> trash;
		*file >> new_plan.angleType >> new_plan.endMovingCoords.x >> new_plan.endMovingCoords.y;
		return new_plan;
	}
	gunPlanExemplar readRotate(ifstream *file, gunPlanExemplar new_plan) {
		string trash;
		*file >> trash;
		new_plan.endTime = read_time(file);
		*file >> trash;
		*file >> new_plan.angleType >> new_plan.gunEndAngle >> new_plan.isRotateClockwise;
		return new_plan;
	}
	gunPlanExemplar readLaserShoot(ifstream *file, gunPlanExemplar new_plan) {
		string trash;
		*file >> trash >> new_plan.startMovingType >> new_plan.startMovingCoords.x >> new_plan.startMovingCoords.y;
		*file >> trash;
		new_plan.laserPreparingEndTime = read_time(file);
		*file >> trash;
		new_plan.endTime = read_time(file);
		*file >> trash;
		*file >> new_plan.angleType >> new_plan.shootAngle >> new_plan.laserSize;
		return new_plan;
	}
	gunPlanExemplar readBulletShoot(ifstream *file, gunPlanExemplar new_plan) {
		string trash;
		string public_or_local;
		*file >> trash >> new_plan.startMovingType >> new_plan.startMovingCoords.x >> new_plan.startMovingCoords.y >> public_or_local;
		if (public_or_local == "lb") {

			new_plan.bulletInfo.timeType.resize(0);
			new_plan.bulletInfo.bulletActionWithWalls.resize(0);
			new_plan.bulletInfo.bulletSize.resize(0);

			new_plan.bulletInfo.accelAngleType.resize(0);
			new_plan.bulletInfo.accelOffsetCoord.resize(0);
			new_plan.bulletInfo.bulletAccelAngle.resize(0);

			new_plan.bulletInfo.speedAngleType.resize(0);
			new_plan.bulletInfo.speedOffsetCoord.resize(0);
			new_plan.bulletInfo.bulletSpeedAngle.resize(0);
			new_plan.bulletInfo.speedChangeType.resize(0);

			new_plan.bulletInfo.lineBulletSpeed.resize(0);
			new_plan.bulletInfo.lineBulletAccel.resize(0);

			new_plan.bulletInfo.bulletSkin.resize(0);
			new_plan.bulletInfo.bulletColor.resize(0);

			*file >> trash;
			int i = 0;
			do {
				int bulletColor, startTime;
				char timeType, bulletActionWithWalls, speedChangeType;
				string accelAngleType, speedAngleType, bulletSkin;
				float bulletSize, bulletAccelAngle, bulletSpeedAngle, lineBulletSpeed, lineBulletAccel;
				sf::Vector2f accelOffsetCoord, speedOffsetCoord;
				
				*file >> timeType;

				
				if (timeType != 'w' && i != 0) startTime = read_time(file);
				else startTime = -1;
				



				*file >> trash >> bulletSize >> trash >> bulletActionWithWalls >> trash >> speedAngleType >> bulletSpeedAngle;
				if (speedAngleType != "abs" && speedAngleType != "rel" && speedAngleType != "rand") 
					*file >> speedOffsetCoord.x >> speedOffsetCoord.y;
				*file >> speedChangeType >> lineBulletSpeed;

				*file >> trash >> accelAngleType >> bulletAccelAngle;
				if (accelAngleType != "abs" && accelAngleType != "sabs" && accelAngleType != "rel" && accelAngleType != "srel" && accelAngleType != "rand") 
					*file >> accelOffsetCoord.x >> accelOffsetCoord.y;
				*file >> lineBulletAccel;

				*file >> trash >> bulletSkin >> bulletColor;

				new_plan.bulletInfo.startTime.push_back(startTime);
				new_plan.bulletInfo.timeType.push_back(timeType);
				new_plan.bulletInfo.bulletActionWithWalls.push_back(bulletActionWithWalls);
				new_plan.bulletInfo.bulletSize.push_back(bulletSize);

				new_plan.bulletInfo.accelAngleType.push_back(accelAngleType);
				new_plan.bulletInfo.accelOffsetCoord.push_back(accelOffsetCoord);
				new_plan.bulletInfo.bulletAccelAngle.push_back(bulletAccelAngle);

				new_plan.bulletInfo.speedAngleType.push_back(speedAngleType);
				new_plan.bulletInfo.speedOffsetCoord.push_back(speedOffsetCoord);
				new_plan.bulletInfo.bulletSpeedAngle.push_back(bulletSpeedAngle);
				new_plan.bulletInfo.speedChangeType.push_back(speedChangeType);

				new_plan.bulletInfo.lineBulletSpeed.push_back(lineBulletSpeed);
				new_plan.bulletInfo.lineBulletAccel.push_back(lineBulletAccel);

				new_plan.bulletInfo.bulletColor.push_back(bulletColor);
				new_plan.bulletInfo.bulletSkin.push_back(bulletSkin);
				*file >> trash;
				i++;
			} while (trash != "}");	
		}
		else if (public_or_local == "pb") {
			int id;
			*file >> id;
			new_plan.bulletInfo.timeType = publicInfo[id].bulletInfo.timeType;
			new_plan.bulletInfo.startTime = publicInfo[id].bulletInfo.startTime;

			new_plan.bulletInfo.bulletActionWithWalls = publicInfo[id].bulletInfo.bulletActionWithWalls;
			new_plan.bulletInfo.bulletSize = publicInfo[id].bulletInfo.bulletSize;

			new_plan.bulletInfo.accelAngleType = publicInfo[id].bulletInfo.accelAngleType;
			new_plan.bulletInfo.accelOffsetCoord = publicInfo[id].bulletInfo.accelOffsetCoord;
			new_plan.bulletInfo.bulletAccelAngle = publicInfo[id].bulletInfo.bulletAccelAngle;

			new_plan.bulletInfo.speedAngleType = publicInfo[id].bulletInfo.speedAngleType;
			new_plan.bulletInfo.speedOffsetCoord = publicInfo[id].bulletInfo.speedOffsetCoord;
			new_plan.bulletInfo.bulletSpeedAngle = publicInfo[id].bulletInfo.bulletSpeedAngle;
			new_plan.bulletInfo.speedChangeType = publicInfo[id].bulletInfo.speedChangeType;

			new_plan.bulletInfo.lineBulletSpeed = publicInfo[id].bulletInfo.lineBulletSpeed;
			new_plan.bulletInfo.lineBulletAccel = publicInfo[id].bulletInfo.lineBulletAccel;

			new_plan.bulletInfo.bulletColor = publicInfo[id].bulletInfo.bulletColor;
			new_plan.bulletInfo.bulletSkin = publicInfo[id].bulletInfo.bulletSkin;
		}
		return new_plan;
	}

	camPlanExemplar readCam(ifstream *file) {
		camPlanExemplar new_plan;
		string command_type, trash;
		*file >> new_plan.type;
		if (new_plan.type == "rotate") {
			*file >> trash;
			new_plan.b_end_time = read_time(file);
			*file >> trash;
			*file >> new_plan.end_angle >> new_plan.is_rotate_direction_clockwise;
		}
		else if (new_plan.type == "zoom") {
			*file >> trash;
			new_plan.b_end_time = read_time(file);
			*file >> trash;
			*file >> new_plan.zoom;
		}
		else if (new_plan.type == "follow") {
			*file >> trash;
			new_plan.b_end_time = read_time(file);
			*file >> trash;
			new_plan.is_player_center = true;
		}
		else if (new_plan.type == "flashlight") {
			*file >> trash;
			new_plan.b_start_animation = read_time(file);
			*file >> trash;
			new_plan.b_end_time = read_time(file);
			*file >> trash;
			new_plan.b_end_animation = read_time(file);
			*file >> trash;
			*file >> new_plan.flashlight_size;
			new_plan.is_player_center = true;
		}
		else if (new_plan.type == "blink") {
			*file >> trash;
			new_plan.b_start_animation = read_time(file);
			*file >> trash;
			new_plan.b_end_time = read_time(file);
			*file >> trash;
			new_plan.b_end_animation = read_time(file);
			*file >> trash;
			*file >> new_plan.blinkAlpha;
		}
		return new_plan;
	}

	zonePlanExemplar readZone(ifstream *file) {
		zonePlanExemplar new_plan;
		string trash;
		*file >> new_plan.type >> trash;
		*file >> new_plan.startBeat >> trash >> new_plan.endBeat >> trash;
		*file >> new_plan.UpLeft.x >> new_plan.UpLeft.y >> trash >> new_plan.DownRight.x >> new_plan.DownRight.y;
		return new_plan;
	}

	BPMchangeExemplar readBPMchange(ifstream *file) {
		BPMchangeExemplar new_plan;
		*file >> new_plan.offset >> new_plan.bpm;
		return new_plan;
	}
	void setNewPublicBullet(ifstream *file) {
		string trash;
		int id;
		bulletPlanExemplar n;
		*file >> id >> trash;
		publicInfo[id].bulletInfo = n;

		publicInfo[id].bulletInfo.startTime.resize(0);
		publicInfo[id].bulletInfo.timeType.resize(0);
		publicInfo[id].bulletInfo.bulletActionWithWalls.resize(0);
		publicInfo[id].bulletInfo.bulletSize.resize(0);

		publicInfo[id].bulletInfo.accelAngleType.resize(0);
		publicInfo[id].bulletInfo.accelOffsetCoord.resize(0);
		publicInfo[id].bulletInfo.bulletAccelAngle.resize(0);

		publicInfo[id].bulletInfo.speedAngleType.resize(0);
		publicInfo[id].bulletInfo.speedOffsetCoord.resize(0);
		publicInfo[id].bulletInfo.bulletSpeedAngle.resize(0);
		publicInfo[id].bulletInfo.speedChangeType.resize(0);

		publicInfo[id].bulletInfo.lineBulletSpeed.resize(0);
		publicInfo[id].bulletInfo.lineBulletAccel.resize(0);

		publicInfo[id].bulletInfo.bulletColor.resize(0);

		int i = 0;
		do {
			int bulletColor, startTime;
			char timeType, bulletActionWithWalls, speedChangeType;
			string accelAngleType, speedAngleType, bulletSkin;
			float bulletSize, bulletAccelAngle, bulletSpeedAngle, lineBulletSpeed, lineBulletAccel;
			sf::Vector2f accelOffsetCoord, speedOffsetCoord;

			*file >> timeType;

			
			if (timeType != 'w' && i != 0) startTime = read_time(file);
			else startTime = -1;
			



			*file >> trash >> bulletSize >> trash >> bulletActionWithWalls >> trash >> speedAngleType >> bulletSpeedAngle;
			if (speedAngleType != "abs" && speedAngleType != "rel" && speedAngleType != "rand")
				*file >> speedOffsetCoord.x >> speedOffsetCoord.y;
			*file >> speedChangeType >> lineBulletSpeed;

			*file >> trash >> accelAngleType >> bulletAccelAngle;
			if (accelAngleType != "abs" && accelAngleType != "sabs" && accelAngleType != "rel" && accelAngleType != "srel" && accelAngleType != "rand")
				*file >> accelOffsetCoord.x >> accelOffsetCoord.y;
			*file >> lineBulletAccel;

			*file >> trash >> bulletSkin >> bulletColor;

		
			publicInfo[id].bulletInfo.startTime.push_back(startTime);
			

			publicInfo[id].bulletInfo.timeType.push_back(timeType);
			publicInfo[id].bulletInfo.bulletActionWithWalls.push_back(bulletActionWithWalls);
			publicInfo[id].bulletInfo.bulletSize.push_back(bulletSize);

			publicInfo[id].bulletInfo.accelAngleType.push_back(accelAngleType);
			publicInfo[id].bulletInfo.accelOffsetCoord.push_back(accelOffsetCoord);
			publicInfo[id].bulletInfo.bulletAccelAngle.push_back(bulletAccelAngle);

			publicInfo[id].bulletInfo.speedAngleType.push_back(speedAngleType);
			publicInfo[id].bulletInfo.speedOffsetCoord.push_back(speedOffsetCoord);
			publicInfo[id].bulletInfo.bulletSpeedAngle.push_back(bulletSpeedAngle);
			publicInfo[id].bulletInfo.speedChangeType.push_back(speedChangeType);

			publicInfo[id].bulletInfo.lineBulletSpeed.push_back(lineBulletSpeed);
			publicInfo[id].bulletInfo.lineBulletAccel.push_back(lineBulletAccel);

			publicInfo[id].bulletInfo.bulletColor.push_back(bulletColor);
			publicInfo[id].bulletInfo.bulletSkin.push_back(bulletSkin);

			*file >> trash;
			i++;
		} while (trash != "}");

	}

	int read_time(ifstream *file) {
		int beats4, beat, beat1_2, beat1_3, beat1_4, beat1_6, beat1_8, beat1_16, beat1_32;
		*file >> beats4 >> beat >> beat1_2 >> beat1_3 >> beat1_4 >> beat1_6 >> beat1_8 >> beat1_16 >> beat1_32;
		return beats4 * 128 + beat * 32 + beat1_2 * 16 + beat1_3 * 11 + beat1_4 * 8 + beat1_6 * 5 + beat1_8 * 4 + beat1_16 * 2 + beat1_32;
	}
};

