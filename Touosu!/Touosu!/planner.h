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
	char startMovingType = 'a';
	sf::Vector2f startCoords;
	float spawnOffsetAngle;
};
struct gunPlanExemplar {
	bulletPlanExemplar bulletInfo;
	vector<bulletPlanExemplar> childBullets;
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
struct startPosExemplar {
	int beatStartTime = 0;
	float startTime = 0;
};

class planner {
public:
	void init() {
		zonePlanList.resize(0);
		camPlanList.resize(0);
		publicInfo.resize(0);
		publicBullets.resize(0);
		numberOfGuns = 0;
		ifstream file;
		file.open("plan.txt");
		file >> numberOfGuns >> randomSeed;
		gunPlanList.resize(numberOfGuns);
		currentGunStep.resize(numberOfGuns, 0);
		currentGunStep[0] = 0;
		currentCamStep = 0;
		forSteps = 0;
		readFile(&file);
		for (unsigned int i = 0; i < gunPlanList.size(); i++)
			sort(gunPlanList[i].begin(), gunPlanList[i].end(), [](const gunPlanExemplar& plan1, const gunPlanExemplar& plan2) -> bool {
				return plan1.startTime < plan2.startTime;
			});

		sort(zonePlanList.begin(), zonePlanList.end(), [](const zonePlanExemplar& plan1, const zonePlanExemplar& plan2) -> bool {
			return plan1.startBeat < plan2.startBeat;
		});
		sort(camPlanList.begin(), camPlanList.end(), [](const camPlanExemplar& plan1, const camPlanExemplar& plan2) -> bool {
			return plan1.b_start_time < plan2.b_start_time;
		});
		startPosInit();
		file.close();
	};

	pair<bool, gunPlanExemplar> getGunPlan(int gunID) {
		pair<bool, gunPlanExemplar> forReturn;
		currentGunStep[gunID]++;
		if (gunPlanList[gunID].size() >= currentGunStep[gunID]) {
			forReturn.first = true;
			forReturn.second = gunPlanList[gunID][currentGunStep[gunID] - 1];
		}
		else forReturn.first = false;
		return forReturn;
	}
	pair<bool, camPlanExemplar> getCamPlan() {
		pair<bool, camPlanExemplar> forReturn;
		currentCamStep++;
		if (camPlanList.size() >= currentCamStep) {
			forReturn.first = true;
			forReturn.second = camPlanList[currentCamStep - 1];
		}
		else forReturn.first = false;
		return forReturn;
	}
	vector<zonePlanExemplar> getZonePlan() {
		return zonePlanList;
	}
	vector<BPMchangeExemplar> getBPMchangesPlan() {
		return BPMchangePlanList;
	}
	startPosExemplar getStartPos() {
		return startPos;
	}
	int getRandomSeed() {
		return randomSeed;
	}
	int getNumberOfGuns() {
		return numberOfGuns;
	}

	void restart() {
		currentGunStep.resize(numberOfGuns, 0);
		currentGunStep[0] = 0;
		currentCamStep = 0;
	}

private:
	vector<vector<gunPlanExemplar>> gunPlanList;
	vector<zonePlanExemplar> zonePlanList;
	vector<camPlanExemplar> camPlanList;
	vector<gunPlanExemplar> publicInfo, publicBullets;
	vector<BPMchangeExemplar> BPMchangePlanList;
	vector<unsigned int> currentGunStep;
	startPosExemplar startPos;
	unsigned int currentCamStep;
	unsigned int numberOfGuns, randomSeed;
	int forSteps;

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
		
		else if (command_type == "public") readPublicVar(file);
		else if (command_type == "new_pb") setNewPublicBullet(file);
		else if (command_type == "for") {
			int numOfActions;
			int selfStep = forSteps;
			forSteps++;
			string trash;
			*file >> numOfActions >> trash;

			ofstream bufferFile;
			string selfFileName = to_string(selfStep) + ".txt";
			bufferFile.open(selfFileName);
			int i = 0;
			while (true) {
				*file >> trash;
				if (trash == "{") i++;
				if (trash == "}") i--;
				if (i < 0) break;
				bufferFile << trash << " ";
			}
			bufferFile << "end";
			bufferFile.close();
			ifstream readFromBufferFile;

			for (int i = 0; i < numOfActions; i++) {
				readFromBufferFile.open(selfFileName);
				readFile(&readFromBufferFile);
				readFromBufferFile.close();
			}
			remove(selfFileName.c_str());
		}
		else if (command_type == "start_pos") readStartPos(file);
		return command_type;
	}

	gunPlanExemplar createNewPlanExemplar(ifstream *file) {
		gunPlanExemplar new_plan;
		string trash;
		char p_or_l;
		*file >> trash >> p_or_l;
		if (p_or_l == 'l') new_plan.startTime = read_time(file);
		else {
			int ID;
			*file >> ID;
			new_plan.startTime = publicInfo[ID].startTime;
		}
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
		char p_or_l;
		*file >> new_plan.endMovingCoords.x >> new_plan.endMovingCoords.y;
		*file >> p_or_l;
		if (p_or_l == 'l') *file >> new_plan.gunAngle;
		else {
			int id;
			*file >> id;
			new_plan.gunAngle = publicInfo[id].gunAngle;
		}
		return new_plan;
	}
	gunPlanExemplar readMove(ifstream *file, gunPlanExemplar new_plan) {
		char p_or_l;
		string trash;
		*file >> trash >> p_or_l;
		if (p_or_l == 'l') new_plan.endTime = read_time(file);
		else {
			int id;
			*file >> id;
			new_plan.endTime = publicInfo[id].endTime;
		}
		*file >> trash;
		*file >> new_plan.angleType >> new_plan.endMovingCoords.x >> new_plan.endMovingCoords.y;
		return new_plan;
	}
	gunPlanExemplar readRotate(ifstream *file, gunPlanExemplar new_plan) {
		string trash;
		char p_or_l;
		*file >> trash >> p_or_l;
		if (p_or_l == 'l') new_plan.endTime = read_time(file);
		else {
			int ID;
			*file >> ID;
			new_plan.endTime = publicInfo[ID].endTime;
		}
		*file >> trash;
		*file >> new_plan.angleType >> p_or_l;
		if (p_or_l == 'l') *file >> new_plan.gunEndAngle;
		else {
			int ID;
			*file >> ID;
			new_plan.gunEndAngle = publicInfo[ID].gunEndAngle;
		}
		*file >> new_plan.isRotateClockwise;
		return new_plan;
	}
	gunPlanExemplar readLaserShoot(ifstream *file, gunPlanExemplar new_plan) {
		string trash;
		char p_or_l;
		int ID;
		*file >> trash >> new_plan.startMovingType;
		*file >> p_or_l;
		if (p_or_l == 'l') *file >> new_plan.startCoords.x;
		else {
			*file >> ID;
			new_plan.startCoords.x = publicInfo[ID].startCoords.x;
		}
		*file >> p_or_l;
		if (p_or_l == 'l') *file >> new_plan.startCoords.y;
		else {
			*file >> ID;
			new_plan.startCoords.y = publicInfo[ID].startCoords.y;
		}
		*file >> p_or_l;
		if (p_or_l == 'l') *file >> new_plan.spawnOffsetAngle;
		else {
			*file >> ID;
			new_plan.spawnOffsetAngle = publicInfo[ID].spawnOffsetAngle;
		}
		*file >> trash >> p_or_l;
		if (p_or_l == 'l') new_plan.laserPreparingEndTime = read_time(file);
		else {
			*file >> ID;
			new_plan.laserPreparingEndTime = publicInfo[ID].laserPreparingEndTime;
		}
		*file >> trash >> p_or_l;
		if (p_or_l == 'l') new_plan.endTime = read_time(file);
		else {
			*file >> ID;
			new_plan.endTime = publicInfo[ID].endTime;
		}
		*file >> trash;
		*file >> new_plan.angleType >> p_or_l;
		if (p_or_l == 'l') *file >> new_plan.laserShootAngle;
		else {
			*file >> ID;
			new_plan.laserShootAngle = publicInfo[ID].laserShootAngle;
		}
		*file >> p_or_l;
		if (p_or_l == 'l') *file >> new_plan.laserSize;
		else {
			*file >> ID;
			new_plan.laserSize = publicInfo[ID].laserSize;
		}
		return new_plan;
	}
	gunPlanExemplar readBulletShoot(ifstream *file, gunPlanExemplar new_plan) {
		string trash;
		string public_or_local;
		char p_or_l;
		int ID;
		*file >> trash >> new_plan.startMovingType;

		*file >> p_or_l;
		if(p_or_l == 'l') *file >> new_plan.startCoords.x;
		else {
			*file >> ID;
			new_plan.startCoords.x = publicInfo[ID].startCoords.x;
		}
		*file >> p_or_l;
		if (p_or_l == 'l') *file >> new_plan.startCoords.y;
		else {
			*file >> ID;
			new_plan.startCoords.y = publicInfo[ID].startCoords.y;
		}
		*file >> p_or_l;
		if (p_or_l == 'l') *file >> new_plan.spawnOffsetAngle;
		else {
			*file >> ID;
			new_plan.spawnOffsetAngle = publicInfo[ID].spawnOffsetAngle;
		}

		new_plan.bulletInfo.startMovingType = new_plan.startMovingType;
		new_plan.bulletInfo.spawnOffsetAngle = new_plan.spawnOffsetAngle;
		new_plan.bulletInfo.startCoords = new_plan.startCoords;

		*file >> public_or_local;
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

			new_plan.childBullets.resize(0);

			*file >> trash;
			int i = 0;
			do {
				string switchOrShoot;
				*file >> switchOrShoot;
				if (switchOrShoot == "switch") {
					int bulletColor, startTime;
					char timeType, bulletActionWithWalls, speedChangeType;
					string accelAngleType, speedAngleType, bulletSkin;
					float bulletSize, bulletAccelAngle, bulletSpeedAngle, lineBulletSpeed, lineBulletAccel;
					sf::Vector2f accelOffsetCoord, speedOffsetCoord;


					*file >> trash >> timeType;


					if (timeType != 'w' && i != 0) {
						*file >> p_or_l;
						if (p_or_l == 'l') startTime = read_time(file);
						else {
							*file >> ID;
							startTime = publicInfo[ID].startTime;
						}
					}
					else startTime = -1;




					*file >> trash >> p_or_l;
					if (p_or_l == 'l') *file >> bulletSize;
					else {
						*file >> ID;
						bulletSize = publicInfo[ID].bulletInfo.bulletSize[0];
					}

					*file >> trash >> bulletActionWithWalls >> trash >> speedAngleType >> p_or_l;
					if (p_or_l == 'l') *file >> bulletSpeedAngle;
					else {
						*file >> ID;
						bulletSpeedAngle = publicInfo[ID].bulletInfo.bulletSpeedAngle[0];
					}

					if (speedAngleType != "abs" && speedAngleType != "rel" && speedAngleType != "rand")
						*file >> speedOffsetCoord.x >> speedOffsetCoord.y;

					*file >> speedChangeType >> p_or_l;
					if (p_or_l == 'l') *file >> lineBulletSpeed;
					else {
						*file >> ID;
						lineBulletSpeed = publicInfo[ID].bulletInfo.lineBulletSpeed[0];
					}

					*file >> trash >> accelAngleType >> p_or_l;
					if (p_or_l == 'l') *file >> bulletAccelAngle;
					else {
						*file >> ID;
						bulletAccelAngle = publicInfo[ID].bulletInfo.bulletAccelAngle[0];
					}
					if (accelAngleType != "abs" && accelAngleType != "sabs" && accelAngleType != "rel" && accelAngleType != "srel" && accelAngleType != "rand")
						*file >> accelOffsetCoord.x >> accelOffsetCoord.y;
					*file >> p_or_l;
					if (p_or_l == 'l') *file >> lineBulletAccel;
					else {
						*file >> ID;
						lineBulletAccel = publicInfo[ID].bulletInfo.lineBulletAccel[0];
					}

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
				}
				else if (switchOrShoot == "shoot") {
					gunPlanExemplar ChildPlan;
					char p_or_l, timeType;
					*file >> trash;
					*file >> timeType;
					int t = -1;
					if (timeType != 'w') {
						*file >> p_or_l;
						int ID;
						if (p_or_l == 'l') t = read_time(file);
						else {
							*file >> ID;
							t = publicInfo[ID].startTime;
						}
					}
					ChildPlan = readBulletShoot(file, ChildPlan);
					ChildPlan.bulletInfo.startTime[0] = t;
					ChildPlan.bulletInfo.timeType[0] = timeType;
					new_plan.childBullets.push_back(ChildPlan.bulletInfo);
					*file >> trash;
				}
			} while (trash != ")");	
		}
		else if (public_or_local == "pb") {
			int id;
			*file >> id;
			new_plan.bulletInfo.timeType = publicBullets[id].bulletInfo.timeType;
			new_plan.bulletInfo.startTime = publicBullets[id].bulletInfo.startTime;

			new_plan.bulletInfo.bulletActionWithWalls = publicBullets[id].bulletInfo.bulletActionWithWalls;
			new_plan.bulletInfo.bulletSize = publicBullets[id].bulletInfo.bulletSize;

			new_plan.bulletInfo.accelAngleType = publicBullets[id].bulletInfo.accelAngleType;
			new_plan.bulletInfo.accelOffsetCoord = publicBullets[id].bulletInfo.accelOffsetCoord;
			new_plan.bulletInfo.bulletAccelAngle = publicBullets[id].bulletInfo.bulletAccelAngle;

			new_plan.bulletInfo.speedAngleType = publicBullets[id].bulletInfo.speedAngleType;
			new_plan.bulletInfo.speedOffsetCoord = publicBullets[id].bulletInfo.speedOffsetCoord;
			new_plan.bulletInfo.bulletSpeedAngle = publicBullets[id].bulletInfo.bulletSpeedAngle;
			new_plan.bulletInfo.speedChangeType = publicBullets[id].bulletInfo.speedChangeType;

			new_plan.bulletInfo.lineBulletSpeed = publicBullets[id].bulletInfo.lineBulletSpeed;
			new_plan.bulletInfo.lineBulletAccel = publicBullets[id].bulletInfo.lineBulletAccel;

			new_plan.bulletInfo.bulletColor = publicBullets[id].bulletInfo.bulletColor;
			new_plan.bulletInfo.bulletSkin = publicBullets[id].bulletInfo.bulletSkin;

			new_plan.childBullets = publicBullets[id].childBullets;
		}
		new_plan.startTime -= 32;
		return new_plan;
	}

	camPlanExemplar readCam(ifstream *file) {
		camPlanExemplar new_plan;
		string command_type, trash;
		*file >> new_plan.type >> trash;
		char p_or_l;
		int ID;
		*file >> p_or_l;
		if (p_or_l == 'l') new_plan.b_start_time = read_time(file);
		else {
			*file >> ID;
			new_plan.b_start_time = publicInfo[ID].startTime;
		}
		*file >> trash >> p_or_l;
		if (p_or_l == 'l') new_plan.b_end_time = read_time(file);
		else {
			*file >> ID;
			new_plan.b_end_time = publicInfo[ID].endTime;
		}
		if (new_plan.type == "rotate") {
			*file >> trash;
			*file >> new_plan.end_angle >> new_plan.is_rotate_direction_clockwise;
		}
		else if (new_plan.type == "zoom") {
			*file >> trash;
			*file >> new_plan.zoom;
		}
		else if (new_plan.type == "follow") {
			new_plan.is_player_center = true;
		}
		else if (new_plan.type == "flashlight") {
			*file >> trash;
			new_plan.b_start_animation = read_time(file);
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
			new_plan.b_end_animation = read_time(file);
			*file >> trash;
			*file >> new_plan.blinkAlpha;
		}
		return new_plan;
	}
	zonePlanExemplar readZone(ifstream *file) {
		zonePlanExemplar new_plan;
		string trash;
		char p_or_l;
		int ID;
		*file >> new_plan.type >> trash >> p_or_l;
		if (p_or_l == 'l') new_plan.startBeat = read_time(file);
		else {
			*file >> ID;
			new_plan.startBeat = publicInfo[ID].startTime;
		}
		*file >> trash >> p_or_l;
		if (p_or_l == 'l') new_plan.endBeat = read_time(file);
		else {
			*file >> ID;
			new_plan.endBeat = publicInfo[ID].endTime;
		}
		*file >> trash;
		*file >> new_plan.UpLeft.x >> new_plan.UpLeft.y >> trash >> new_plan.DownRight.x >> new_plan.DownRight.y;
		return new_plan;
	}
	BPMchangeExemplar readBPMchange(ifstream *file) {
		BPMchangeExemplar new_plan;
		*file >> new_plan.offset >> new_plan.bpm;
		return new_plan;
	}

	void readStartPos(ifstream *file) {
		startPos.beatStartTime = read_time(file);
	}
	void startPosInit() {
		if (startPos.beatStartTime == 0) return;
		startPos.startTime = BPMchangePlanList[0].offset;
		float bpm = BPMchangePlanList[0].bpm;
		float time = BPMtoMCSdiv600Converter / bpm;
		int bufferBeat = 0;
		while (bufferBeat != startPos.beatStartTime) {
			startPos.startTime += time * 600;
			bufferBeat++;
			if (BPMchangePlanList.size() > 1 && BPMchangePlanList[1].offset < startPos.startTime) {
				bufferBeat--;
				bpm = BPMchangePlanList[1].bpm;
				time = BPMtoMCSdiv600Converter / bpm;
				BPMchangePlanList.erase(BPMchangePlanList.begin());
			}
		}
		for (int i = 0; i < gunPlanList.size(); i++) while (true) {
			if (gunPlanList[i].size() == 0 || gunPlanList[i][0].startTime >= startPos.beatStartTime) break;
			else gunPlanList[i].erase(gunPlanList[i].begin());
		}
		while (true) {
			if (zonePlanList.size() == 0 || zonePlanList[0].startBeat >= startPos.beatStartTime) break;
			else zonePlanList.erase(zonePlanList.begin());
		}
		while (true) {
			if (camPlanList.size() == 0 || camPlanList[0].b_start_time >= startPos.beatStartTime) break;
			else camPlanList.erase(camPlanList.begin());
		}
	}

	void readPublicVar(ifstream *file) {
		string varName; unsigned int varID; char oper;
		*file >> varName >> varID >> oper;
		if (varID == publicInfo.size()) {
			publicInfo.resize(varID + 1);
			publicInfo[varID].bulletInfo.bulletAccelAngle.resize(1);
			publicInfo[varID].bulletInfo.bulletSize.resize(1);
			publicInfo[varID].bulletInfo.bulletSpeedAngle.resize(1);
			publicInfo[varID].bulletInfo.lineBulletAccel.resize(1);
			publicInfo[varID].bulletInfo.lineBulletSpeed.resize(1);
		}
		switch (oper)
		{
		case '=':
			if (varName == "startTime") publicInfo[varID].startTime = read_time(file);
			else if (varName == "endTime") publicInfo[varID].endTime = read_time(file);
			else if (varName == "laserPreparingEndTime") publicInfo[varID].laserPreparingEndTime = read_time(file);
			else if (varName == "laserSize") *file >> publicInfo[varID].laserSize;
			else if (varName == "gunAngle") *file >> publicInfo[varID].gunAngle;
			else if (varName == "gunEndAngle") *file >> publicInfo[varID].gunEndAngle;
			else if (varName == "laserShootAngle") *file >> publicInfo[varID].laserShootAngle;
			else if (varName == "startCoords.x") *file >> publicInfo[varID].startCoords.x;
			else if (varName == "startCoords.y") *file >> publicInfo[varID].startCoords.y;
			else if (varName == "spawnOffsetAngle") *file >> publicInfo[varID].spawnOffsetAngle;
			else if (varName == "bulletAccelAngle") *file >> publicInfo[varID].bulletInfo.bulletAccelAngle[0];
			else if (varName == "bulletSize") *file >> publicInfo[varID].bulletInfo.bulletSize[0];
			else if (varName == "bulletSpeedAngle") *file >> publicInfo[varID].bulletInfo.bulletSpeedAngle[0];
			else if (varName == "lineBulletAccel") *file >> publicInfo[varID].bulletInfo.lineBulletAccel[0];
			else if (varName == "lineBulletSpeed") *file >> publicInfo[varID].bulletInfo.lineBulletSpeed[0];
			break;
		case '+':
			float a;
			if (varName == "startTime") {
				a = (float)read_time(file);
				publicInfo[varID].startTime += (int)a;
			}
			else if (varName == "endTime") {
				a = (float)read_time(file);
				publicInfo[varID].endTime += (int)a;
			}
			else if (varName == "laserPreparingEndTime") {
				a = (float)read_time(file);
				publicInfo[varID].laserPreparingEndTime += (int)a;
			}
			else if (varName == "laserSize") {
				*file >> a;
				publicInfo[varID].laserSize += a;
			}
			else if (varName == "shootAngle") {
				*file >> a;
				publicInfo[varID].gunAngle += a;
			}
			else if (varName == "gunEndAngle") {
				*file >> a;
				publicInfo[varID].gunEndAngle += a;
			}
			else if (varName == "laserShootAngle") {
				*file >> a;
				publicInfo[varID].laserShootAngle += a;
			}
			else if (varName == "startCoords.x") {
				*file >> a;
				publicInfo[varID].startCoords.x += a;
			}
			else if (varName == "startCoords.y") {
				*file >> a;
				publicInfo[varID].startCoords.y += a;
			}
			else if (varName == "spawnOffsetAngle") {
				*file >> a;
				publicInfo[varID].spawnOffsetAngle += a;
			}
			else if (varName == "bulletAccelAngle"){
				*file >> a;
				publicInfo[varID].bulletInfo.bulletAccelAngle[0] += a;
			}
			else if (varName == "bulletSize") {
				*file >> a;
				publicInfo[varID].bulletInfo.bulletSize[0] += a;
			}
			else if (varName == "bulletSpeedAngle") {
				*file >> a;
				publicInfo[varID].bulletInfo.bulletSpeedAngle[0] += a;
			}
			else if (varName == "lineBulletAccel") {
				*file >> a;
				publicInfo[varID].bulletInfo.lineBulletAccel[0] += a;
			}
			else if (varName == "lineBulletSpeed") {
				*file >> a;
				publicInfo[varID].bulletInfo.lineBulletSpeed[0] += a;
			}
			break;

		case '~':
			if (varName == "laserSize") publicInfo[varID].laserSize += readRandomFloat(file);
			else if (varName == "gunAngle") publicInfo[varID].gunAngle += readRandomFloat(file);
			else if (varName == "gunEndAngle") publicInfo[varID].gunEndAngle += readRandomFloat(file);
			else if (varName == "laserShootAngle") publicInfo[varID].laserShootAngle += readRandomFloat(file);
			else if (varName == "startCoords.x") publicInfo[varID].startCoords.x += readRandomFloat(file);
			else if (varName == "startCoords.y") publicInfo[varID].startCoords.y += readRandomFloat(file);
			else if (varName == "spawnOffsetAngle") publicInfo[varID].spawnOffsetAngle += readRandomFloat(file);
			else if (varName == "bulletAccelAngle") publicInfo[varID].bulletInfo.bulletAccelAngle[0] += readRandomFloat(file);
			else if (varName == "bulletSize") publicInfo[varID].bulletInfo.bulletSize[0] += readRandomFloat(file);
			else if (varName == "bulletSpeedAngle") publicInfo[varID].bulletInfo.bulletSpeedAngle[0] += readRandomFloat(file);
			else if (varName == "lineBulletAccel") publicInfo[varID].bulletInfo.lineBulletAccel[0] += readRandomFloat(file);
			else if (varName == "lineBulletSpeed") publicInfo[varID].bulletInfo.lineBulletSpeed[0] += readRandomFloat(file);
			break;
		}
	}

	void setNewPublicBullet(ifstream *file) {
		string trash;
		int id;
		bulletPlanExemplar n;
		*file >> id >> trash;
		if (id == publicBullets.size()) publicBullets.resize(id + 1);
		publicBullets[id].bulletInfo = n;

		publicBullets[id].bulletInfo.startTime.resize(0);
		publicBullets[id].bulletInfo.timeType.resize(0);
		publicBullets[id].bulletInfo.bulletActionWithWalls.resize(0);
		publicBullets[id].bulletInfo.bulletSize.resize(0);

		publicBullets[id].bulletInfo.accelAngleType.resize(0);
		publicBullets[id].bulletInfo.accelOffsetCoord.resize(0);
		publicBullets[id].bulletInfo.bulletAccelAngle.resize(0);

		publicBullets[id].bulletInfo.speedAngleType.resize(0);
		publicBullets[id].bulletInfo.speedOffsetCoord.resize(0);
		publicBullets[id].bulletInfo.bulletSpeedAngle.resize(0);
		publicBullets[id].bulletInfo.speedChangeType.resize(0);

		publicBullets[id].bulletInfo.lineBulletSpeed.resize(0);
		publicBullets[id].bulletInfo.lineBulletAccel.resize(0);

		publicBullets[id].bulletInfo.bulletColor.resize(0);
		publicBullets[id].childBullets.resize(0);
		int i = 0;
		do {
			string switchOrShoot;
			*file >> switchOrShoot >> trash;
			if (switchOrShoot == "switch") {
				int bulletColor, startTime;
				char timeType, bulletActionWithWalls, speedChangeType;
				string accelAngleType, speedAngleType, bulletSkin;
				float bulletSize, bulletAccelAngle, bulletSpeedAngle, lineBulletSpeed, lineBulletAccel;
				sf::Vector2f accelOffsetCoord, speedOffsetCoord;

				char p_or_l;
				int ID;

				*file >> timeType;


				if (timeType != 'w' && i != 0) {
					*file >> p_or_l;
					if (p_or_l == 'l') startTime = read_time(file);
					else {
						*file >> ID;
						startTime = publicInfo[ID].startTime;
					}
				}
				else startTime = -1;




				*file >> trash >> p_or_l;
				if (p_or_l == 'l') *file >> bulletSize;
				else {
					*file >> ID;
					bulletSize = publicInfo[ID].bulletInfo.bulletSize[0];
				}

				*file >> trash >> bulletActionWithWalls >> trash >> speedAngleType >> p_or_l;
				if (p_or_l == 'l') *file >> bulletSpeedAngle;
				else {
					*file >> ID;
					bulletSpeedAngle = publicInfo[ID].bulletInfo.bulletSpeedAngle[0];
				}

				if (speedAngleType != "abs" && speedAngleType != "rel" && speedAngleType != "rand")
					*file >> speedOffsetCoord.x >> speedOffsetCoord.y;

				*file >> speedChangeType >> p_or_l;
				if (p_or_l == 'l') *file >> lineBulletSpeed;
				else {
					*file >> ID;
					lineBulletSpeed = publicInfo[ID].bulletInfo.lineBulletSpeed[0];
				}

				*file >> trash >> accelAngleType >> p_or_l;
				if (p_or_l == 'l') *file >> bulletAccelAngle;
				else {
					*file >> ID;
					bulletAccelAngle = publicInfo[ID].bulletInfo.bulletAccelAngle[0];
				}
				if (accelAngleType != "abs" && accelAngleType != "sabs" && accelAngleType != "rel" && accelAngleType != "srel" && accelAngleType != "rand")
					*file >> accelOffsetCoord.x >> accelOffsetCoord.y;
				*file >> p_or_l;
				if (p_or_l == 'l') *file >> lineBulletAccel;
				else {
					*file >> ID;
					lineBulletAccel = publicInfo[ID].bulletInfo.lineBulletAccel[0];
				}

				*file >> trash >> bulletSkin >> bulletColor;


				publicBullets[id].bulletInfo.startTime.push_back(startTime);


				publicBullets[id].bulletInfo.timeType.push_back(timeType);
				publicBullets[id].bulletInfo.bulletActionWithWalls.push_back(bulletActionWithWalls);
				publicBullets[id].bulletInfo.bulletSize.push_back(bulletSize);

				publicBullets[id].bulletInfo.accelAngleType.push_back(accelAngleType);
				publicBullets[id].bulletInfo.accelOffsetCoord.push_back(accelOffsetCoord);
				publicBullets[id].bulletInfo.bulletAccelAngle.push_back(bulletAccelAngle);

				publicBullets[id].bulletInfo.speedAngleType.push_back(speedAngleType);
				publicBullets[id].bulletInfo.speedOffsetCoord.push_back(speedOffsetCoord);
				publicBullets[id].bulletInfo.bulletSpeedAngle.push_back(bulletSpeedAngle);
				publicBullets[id].bulletInfo.speedChangeType.push_back(speedChangeType);

				publicBullets[id].bulletInfo.lineBulletSpeed.push_back(lineBulletSpeed);
				publicBullets[id].bulletInfo.lineBulletAccel.push_back(lineBulletAccel);

				publicBullets[id].bulletInfo.bulletColor.push_back(bulletColor);
				publicBullets[id].bulletInfo.bulletSkin.push_back(bulletSkin);

				*file >> trash;
				i++;
			}
			else if (switchOrShoot == "shoot") {
				gunPlanExemplar ChildPlan;
				char p_or_l;
				*file >> trash;
				*file >> p_or_l;
				int t, ID;
				if (p_or_l == 'l') t = read_time(file);
				else {
					*file >> ID;
					t = publicInfo[ID].startTime;
				}
				ChildPlan.bulletInfo.startTime.push_back(t);
				ChildPlan = readBulletShoot(file, ChildPlan);
				publicBullets[id].childBullets.push_back(ChildPlan.bulletInfo);
			}
		} while (trash != "}");

	}

	int read_time(ifstream *file) {
		int beats4, beat, beat1_2, beat1_3, beat1_4, beat1_6, beat1_8, beat1_16, beat1_32;
		*file >> beats4 >> beat >> beat1_2 >> beat1_3 >> beat1_4 >> beat1_6 >> beat1_8 >> beat1_16 >> beat1_32;
		return beats4 * 128 + beat * 32 + beat1_2 * 16 + beat1_3 * 11 + beat1_4 * 8 + beat1_6 * 5 + beat1_8 * 4 + beat1_16 * 2 + beat1_32;
	}

	float readRandomFloat(ifstream *file) {
		float min, max, step;
		string trash;
		*file >> min >> trash >> max >> trash >> step;

		int randRange = (int)(abs(max - min) / step) + 1;
		float out = (float)(rand() % randRange) * step + min;
		return out;
	}
};

