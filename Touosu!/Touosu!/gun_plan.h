#pragma once
#include <fstream>
using namespace std;

struct plan_exemplar {

	//a - absolute music time
	//r - relative music time (from spawn time to action time)
	//w - after touching with wall
	vector<char> timeType;

	vector<int> startTime;
	int endTime = -1, laserPreparingEndTime = -1;
	//'a' for absolute
	//'r' for relative without gunAngle
	//'s' for relative with gunAngle
	char startMovingType = 'a';
	vector<char> angleType;

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
	//"rand" for rangom angle (in rangle from 0 to angle)
	vector<string> accelAngleType;

	//'a' for absolute speed
	//'r' for relative speed
	vector<char> speedChangeType;

	vector<sf::Vector2f> speedOffsetCoord, accelOffsetCoord;
	vector<double> lineBulletSpeed, lineBulletAccel, shootAngle, bulletSize, bulletSpeedAngle, bulletAccelAngle;

	//1-16 color ID, 0 for random
	vector<int> bulletColor;
	//'b' - bounce from walls
	//'d' - destroy by walls
	//'i' - ingnore walls
	//'t' - teleport to opposive wall
	//'c' - classic teleport to opposive wall relative center
	vector<char> bulletActionWithWalls;
	double gunEndAngle = 0, angleSpeed = 0;
	sf::Vector2f startMovingCoords, endMovingCoords, gunSpeed;
	std::string commandType;
	bool isRotateClockwise = false;
};

class gun_plan {
public:
	gun_plan() {}
	gun_plan(int gun_id) {
		plan_list.resize(0);
		current_step = 0;
		ifstream file;
		file.open("plan.txt");
		readFile(&file, gun_id);
		sort(plan_list.begin(), plan_list.end(), [](const plan_exemplar& plan1, const plan_exemplar& plan2) -> bool {
			return plan1.startTime[0] < plan2.startTime[0];
		});
		file.close();
	};

	plan_exemplar get_plan() {
		current_step++;
		if (plan_list.size() >= abs(current_step)) return plan_list[current_step - 1];
		else return plan_list[0];
	}

private:
	vector<plan_exemplar> plan_list;
	plan_exemplar public_bullet;
	int current_step;
	void readFile(ifstream *file, int gun_id) {
		string command_type;
		do {
			string trash;
			*file >> command_type;
			if (command_type == "g") {
				int id;
				*file >> id;
				plan_exemplar new_plan = createNewPlanExemplar(file);
				if (id == gun_id) plan_list.push_back(new_plan);
			}
			else if (command_type == "new_pb") setNewPublicBullet(file);
			else if (command_type == "for") {
				int numberOfActions;
				string trash;
				*file >> numberOfActions >> trash;
				int deltaTime = read_time(file);
				*file >> trash;
				int id;
				*file >> id;
				plan_exemplar new_plan = createNewPlanExemplar(file);
				if (id == gun_id)
					for (int i = 0; i < numberOfActions; i++) {
						plan_list.push_back(new_plan);
						new_plan.startTime[0] += deltaTime;
						new_plan.endTime += deltaTime;
						new_plan.laserPreparingEndTime += deltaTime;
					}
			}
		} while (command_type != "end");
	}

	plan_exemplar createNewPlanExemplar(ifstream *file) {
		plan_exemplar new_plan;
		string trash;
		*file >> trash;
		new_plan.startTime.resize(0);
		new_plan.startTime.push_back(read_time(file));
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

	plan_exemplar readSet(ifstream *file, plan_exemplar new_plan) {
		new_plan.shootAngle.push_back(0);
		*file >> new_plan.endMovingCoords.x >> new_plan.endMovingCoords.y >> new_plan.shootAngle[0];
		return new_plan;
	}
	plan_exemplar readMove(ifstream *file, plan_exemplar new_plan) {
		string trash;
		*file >> trash;
		new_plan.endTime = read_time(file);
		*file >> trash;
		new_plan.angleType.push_back('a');
		*file >> new_plan.angleType[0] >> new_plan.endMovingCoords.x >> new_plan.endMovingCoords.y;
		return new_plan;
	}
	plan_exemplar readRotate(ifstream *file, plan_exemplar new_plan) {
		string trash;
		*file >> trash;
		new_plan.endTime = read_time(file);
		*file >> trash;
		new_plan.angleType.push_back('a');
		*file >> new_plan.angleType[0] >> new_plan.gunEndAngle >> new_plan.isRotateClockwise;
		return new_plan;
	}
	plan_exemplar readLaserShoot(ifstream *file, plan_exemplar new_plan) {
		string trash;
		*file >> trash >> new_plan.startMovingType >> new_plan.startMovingCoords.x >> new_plan.startMovingCoords.y;
		*file >> trash;
		new_plan.laserPreparingEndTime = read_time(file);
		*file >> trash;
		new_plan.endTime = read_time(file);
		*file >> trash;
		new_plan.angleType.push_back('a');
		new_plan.shootAngle.push_back(0);
		new_plan.bulletSize.push_back(0);
		*file >> new_plan.angleType[0] >> new_plan.shootAngle[0] >> new_plan.bulletSize[0];
		return new_plan;
	}
	plan_exemplar readBulletShoot(ifstream *file, plan_exemplar new_plan) {
		string trash;
		string public_or_local;
		*file >> trash >> new_plan.startMovingType >> new_plan.startMovingCoords.x >> new_plan.startMovingCoords.y >> public_or_local;
		if (public_or_local == "lb") {

			new_plan.timeType.resize(0);
			new_plan.bulletActionWithWalls.resize(0);
			new_plan.bulletSize.resize(0);

			new_plan.accelAngleType.resize(0);
			new_plan.accelOffsetCoord.resize(0);
			new_plan.bulletAccelAngle.resize(0);

			new_plan.speedAngleType.resize(0);
			new_plan.speedOffsetCoord.resize(0);
			new_plan.bulletSpeedAngle.resize(0);
			new_plan.speedChangeType.resize(0);

			new_plan.lineBulletSpeed.resize(0);
			new_plan.lineBulletAccel.resize(0);

			new_plan.bulletColor.resize(0);

			*file >> trash;
			int i = 0;
			do {
				int bulletColor, startTime;
				char timeType, bulletActionWithWalls, speedChangeType;
				string accelAngleType, speedAngleType;
				double bulletSize, bulletAccelAngle, bulletSpeedAngle, lineBulletSpeed, lineBulletAccel;
				sf::Vector2f accelOffsetCoord, speedOffsetCoord;
				
				*file >> timeType;

				if (i != 0) {
					if (timeType != 'w' && i != 0) startTime = read_time(file);
					else startTime = -1;
				}



				*file >> trash >> bulletSize >> trash >> bulletActionWithWalls >> trash >> speedAngleType >> bulletSpeedAngle;
				if (speedAngleType != "abs" && speedAngleType != "rel" && speedAngleType != "rand") 
					*file >> speedOffsetCoord.x >> speedOffsetCoord.y;
				*file >> speedChangeType >> lineBulletSpeed;

				*file >> trash >> accelAngleType >> bulletAccelAngle;
				if (accelAngleType != "abs" && accelAngleType != "sabs" && accelAngleType != "rel" && accelAngleType != "srel" && accelAngleType != "rand") 
					*file >> accelOffsetCoord.x >> accelOffsetCoord.y;
				*file >> lineBulletAccel;

				*file >> trash >> bulletColor;

				if (timeType != 'n') {
					new_plan.startTime.push_back(startTime);
				}

				new_plan.timeType.push_back(timeType);
				new_plan.bulletActionWithWalls.push_back(bulletActionWithWalls);
				new_plan.bulletSize.push_back(bulletSize);

				new_plan.accelAngleType.push_back(accelAngleType);
				new_plan.accelOffsetCoord.push_back(accelOffsetCoord);
				new_plan.bulletAccelAngle.push_back(bulletAccelAngle);

				new_plan.speedAngleType.push_back(speedAngleType);
				new_plan.speedOffsetCoord.push_back(speedOffsetCoord);
				new_plan.bulletSpeedAngle.push_back(bulletSpeedAngle);
				new_plan.speedChangeType.push_back(speedChangeType);

				new_plan.lineBulletSpeed.push_back(lineBulletSpeed);
				new_plan.lineBulletAccel.push_back(lineBulletAccel);

				new_plan.bulletColor.push_back(bulletColor);

				*file >> trash;
				i++;
			} while (trash != "}");	
		}


		else if (public_or_local == "pb") {
			new_plan.startMovingType = public_bullet.startMovingType;
			new_plan.startMovingCoords = public_bullet.startMovingCoords;

			new_plan.timeType = public_bullet.timeType;
			new_plan.startTime = public_bullet.startTime;

			new_plan.bulletActionWithWalls = public_bullet.bulletActionWithWalls;
			new_plan.bulletSize = public_bullet.bulletSize;

			new_plan.accelAngleType = public_bullet.accelAngleType;
			new_plan.accelOffsetCoord = public_bullet.accelOffsetCoord;
			new_plan.bulletAccelAngle = public_bullet.bulletAccelAngle;

			new_plan.speedAngleType = public_bullet.speedAngleType;
			new_plan.speedOffsetCoord = public_bullet.speedOffsetCoord;
			new_plan.bulletSpeedAngle = public_bullet.bulletSpeedAngle;
			new_plan.speedChangeType = public_bullet.speedChangeType;

			new_plan.lineBulletSpeed = public_bullet.lineBulletSpeed;
			new_plan.lineBulletAccel = public_bullet.lineBulletAccel;

			new_plan.bulletColor = public_bullet.bulletColor;
		}
		return new_plan;
	}

	void setNewPublicBullet(ifstream *file) {
		string trash;
		plan_exemplar n;
		public_bullet = n;
		*file >> trash >> public_bullet.startMovingType >> public_bullet.startMovingCoords.x >> public_bullet.startMovingCoords.y;
		*file >> trash;

		public_bullet.timeType.resize(0);
		public_bullet.bulletActionWithWalls.resize(0);
		public_bullet.bulletSize.resize(0);

		public_bullet.accelAngleType.resize(0);
		public_bullet.accelOffsetCoord.resize(0);
		public_bullet.bulletAccelAngle.resize(0);

		public_bullet.speedAngleType.resize(0);
		public_bullet.speedOffsetCoord.resize(0);
		public_bullet.bulletSpeedAngle.resize(0);
		public_bullet.speedChangeType.resize(0);

		public_bullet.lineBulletSpeed.resize(0);
		public_bullet.lineBulletAccel.resize(0);

		public_bullet.bulletColor.resize(0);

		int i = 0;
		do {
			int bulletColor, startTime;
			char timeType, bulletActionWithWalls, speedChangeType;
			string accelAngleType, speedAngleType;
			double bulletSize, bulletAccelAngle, bulletSpeedAngle, lineBulletSpeed, lineBulletAccel;
			sf::Vector2f accelOffsetCoord, speedOffsetCoord;

			if (i != 0) {
				*file >> timeType;
				if (timeType != 'w' || i != 0) startTime = read_time(file);
				else startTime = -1;
			}
			else timeType = 'n';
			*file >> trash >> bulletSize >> trash >> bulletActionWithWalls >> trash >> speedAngleType >> bulletSpeedAngle;
			if (speedAngleType != "abs" && speedAngleType != "rel") *file >> speedOffsetCoord.x >> speedOffsetCoord.y;
			*file >> speedChangeType >> lineBulletSpeed;

			*file >> trash >> accelAngleType >> bulletAccelAngle;
			if (accelAngleType != "abs" && accelAngleType != "sabs" && accelAngleType != "rel" && accelAngleType != "srel") *file >> accelOffsetCoord.x >> accelOffsetCoord.y;
			*file >> lineBulletAccel;

			*file >> bulletColor;

			if (timeType != 'n') {
				public_bullet.timeType.push_back(timeType);
				public_bullet.startTime.push_back(startTime);
			}

			public_bullet.bulletActionWithWalls.push_back(bulletActionWithWalls);
			public_bullet.bulletSize.push_back(bulletSize);

			public_bullet.accelAngleType.push_back(accelAngleType);
			public_bullet.accelOffsetCoord.push_back(accelOffsetCoord);
			public_bullet.bulletAccelAngle.push_back(bulletAccelAngle);

			public_bullet.speedAngleType.push_back(speedAngleType);
			public_bullet.speedOffsetCoord.push_back(speedOffsetCoord);
			public_bullet.bulletSpeedAngle.push_back(bulletSpeedAngle);
			public_bullet.speedChangeType.push_back(speedChangeType);

			public_bullet.lineBulletSpeed.push_back(lineBulletSpeed);
			public_bullet.lineBulletAccel.push_back(lineBulletAccel);

			public_bullet.bulletColor.push_back(bulletColor);

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

