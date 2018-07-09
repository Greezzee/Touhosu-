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
	//'r' for relative
	//'p' for angle to player from gun
	//'s' for angle to player from spawn pos
	char startMovingType = 'a';
	vector<char> angleType;

	vector<double> lineBulletSpeed, lineBulletAccel, shootAngle, bulletSize;
	vector<sf::Vector2f> notLineBulletSpeed, notLineBulletAccel;
	vector<std::string> bulletMovmentType;
	//1-16 color ID, 0 for random
	vector<int> bulletColor;
	//'b' - bounce from walls
	//'d' - destroy by walls
	//'i' - ingnore walls
	//'t' - teleport to opposive wall
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
			new_plan.bulletMovmentType.resize(0);
			new_plan.bulletActionWithWalls.resize(0);
			new_plan.bulletSize.resize(0);
			new_plan.angleType.resize(0);
			new_plan.shootAngle.resize(0);
			new_plan.lineBulletSpeed.resize(0);
			new_plan.lineBulletAccel.resize(0);
			new_plan.notLineBulletSpeed.resize(0);
			new_plan.notLineBulletAccel.resize(0);
			new_plan.bulletColor.resize(0);

			*file >> trash;
			int i = 0;
			do {

				new_plan.timeType.push_back('a');
				new_plan.startTime.push_back(0);

				if (new_plan.bulletMovmentType.size() != 0) {
					char timeType;
					*file >> timeType;
					new_plan.timeType[i] = timeType;
					new_plan.startTime[i] = read_time(file);
					*file >> trash;
				}

				new_plan.bulletMovmentType.push_back("");
				new_plan.bulletActionWithWalls.push_back('a');
				new_plan.bulletSize.push_back(0);
				new_plan.angleType.push_back(0);
				new_plan.shootAngle.push_back(0);
				new_plan.lineBulletSpeed.push_back(0);
				new_plan.lineBulletAccel.push_back(0);
				new_plan.notLineBulletSpeed.push_back(sf::Vector2f());
				new_plan.notLineBulletAccel.push_back(sf::Vector2f());
				new_plan.bulletColor.push_back(0);

				string bulletMovmentType;
				char angleType, bulletActionWithWalls;
				double bulletSize, shootAngle;
				int bulletColor;
				*file >> bulletMovmentType >> bulletActionWithWalls >> bulletSize >> angleType >> shootAngle;
				new_plan.bulletMovmentType[i] = bulletMovmentType;
				new_plan.bulletActionWithWalls[i] = bulletActionWithWalls;
				new_plan.bulletSize[i] = bulletSize;
				new_plan.angleType[i] = angleType;
				new_plan.shootAngle[i] = shootAngle;

				if (bulletMovmentType == "line") {
					double lineBulletSpeed, lineBulletAccel;
					*file >> lineBulletSpeed >> lineBulletAccel;
					new_plan.lineBulletSpeed[i] = lineBulletSpeed;
					new_plan.lineBulletAccel[i] = lineBulletAccel;
				}
				else if (bulletMovmentType == "not_line_speed") {
					sf::Vector2f notLineBulletSpeed;
					double lineBulletAccel;
					*file >> notLineBulletSpeed.x >> notLineBulletSpeed.y >> lineBulletAccel;
					new_plan.notLineBulletSpeed[i] = notLineBulletSpeed;
					new_plan.lineBulletAccel[i] = lineBulletAccel;
				}
				else if (bulletMovmentType == "not_line_accel") {
					sf::Vector2f notLineBulletAccel;
					double lineBulletSpeed;
					*file >> lineBulletSpeed >> notLineBulletAccel.x >> notLineBulletAccel.y;
					new_plan.notLineBulletAccel[i] = notLineBulletAccel;
					new_plan.lineBulletSpeed[i] = lineBulletSpeed;
				}
				else if (bulletMovmentType == "not_line") {
					sf::Vector2f notLineBulletAccel, notLineBulletSpeed;
					*file >> notLineBulletSpeed.x >> notLineBulletSpeed.y >> notLineBulletAccel.x >> notLineBulletAccel.y;
					new_plan.notLineBulletSpeed[i] = notLineBulletSpeed;
					new_plan.notLineBulletAccel[i] = notLineBulletAccel;
				}
				*file >> trash >> bulletColor >> trash;
				new_plan.bulletColor[i] = bulletColor;

				i++;
			} while (trash != "}");
			
		}


		else if (public_or_local == "pb") {
			new_plan.bulletSize = public_bullet.bulletSize;
			new_plan.shootAngle = public_bullet.shootAngle;
			new_plan.lineBulletSpeed = public_bullet.lineBulletSpeed;
			new_plan.notLineBulletSpeed = public_bullet.notLineBulletSpeed;
			new_plan.lineBulletAccel = public_bullet.lineBulletAccel;
			new_plan.notLineBulletAccel = public_bullet.notLineBulletAccel;
			new_plan.angleType = public_bullet.angleType;
			new_plan.bulletColor = public_bullet.bulletColor;
			new_plan.startMovingType = public_bullet.startMovingType;
			new_plan.startMovingCoords = public_bullet.startMovingCoords;
			new_plan.timeType = public_bullet.timeType;
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
		public_bullet.startTime.resize(0);
		public_bullet.bulletMovmentType.resize(0);
		public_bullet.bulletActionWithWalls.resize(0);
		public_bullet.bulletSize.resize(0);
		public_bullet.angleType.resize(0);
		public_bullet.shootAngle.resize(0);
		public_bullet.lineBulletSpeed.resize(0);
		public_bullet.lineBulletAccel.resize(0);
		public_bullet.notLineBulletSpeed.resize(0);
		public_bullet.notLineBulletAccel.resize(0);
		public_bullet.bulletColor.resize(0);

		int i = 0;
		do {

			public_bullet.timeType.push_back('a');
			public_bullet.startTime.push_back(0);

			if (public_bullet.bulletMovmentType.size() != 0) {
				char timeType;
				*file >> timeType;
				public_bullet.timeType.push_back(timeType);
				public_bullet.startTime.push_back(read_time(file));
				*file >> trash;
			}

			public_bullet.bulletMovmentType.push_back("");
			public_bullet.bulletActionWithWalls.push_back('a');
			public_bullet.bulletSize.push_back(0);
			public_bullet.angleType.push_back(0);
			public_bullet.shootAngle.push_back(0);
			public_bullet.lineBulletSpeed.push_back(0);
			public_bullet.lineBulletAccel.push_back(0);
			public_bullet.notLineBulletSpeed.push_back(sf::Vector2f());
			public_bullet.notLineBulletAccel.push_back(sf::Vector2f());
			public_bullet.bulletColor.push_back(0);

			string bulletMovmentType;
			char angleType, bulletActionWithWalls;
			double bulletSize, shootAngle;
			int bulletColor;
			*file >> bulletMovmentType >> bulletActionWithWalls >> bulletSize >> angleType >> shootAngle;
			public_bullet.bulletMovmentType[i] = bulletMovmentType;
			public_bullet.bulletActionWithWalls[i] = bulletActionWithWalls;
			public_bullet.bulletSize[i] = bulletSize;
			public_bullet.angleType[i] = angleType;
			public_bullet.shootAngle[i] = shootAngle;

			if (bulletMovmentType == "line") {
				double lineBulletSpeed, lineBulletAccel;
				*file >> lineBulletSpeed >> lineBulletAccel;
				public_bullet.lineBulletSpeed[i] = lineBulletSpeed;
				public_bullet.lineBulletAccel[i] = lineBulletAccel;
			}
			else if (bulletMovmentType == "not_line_speed") {
				sf::Vector2f notLineBulletSpeed;
				double lineBulletAccel;
				*file >> notLineBulletSpeed.x >> notLineBulletSpeed.y >> lineBulletAccel;
				public_bullet.notLineBulletSpeed[i] = notLineBulletSpeed;
				public_bullet.lineBulletAccel[i] = lineBulletAccel;
			}
			else if (bulletMovmentType == "not_line_accel") {
				sf::Vector2f notLineBulletAccel;
				double lineBulletSpeed;
				*file >> lineBulletSpeed >> notLineBulletAccel.x >> notLineBulletAccel.y;
				public_bullet.notLineBulletAccel[i] = notLineBulletAccel;
				public_bullet.lineBulletSpeed[i] = lineBulletSpeed;
			}
			else if (bulletMovmentType == "not_line") {
				sf::Vector2f notLineBulletAccel, notLineBulletSpeed;
				*file >> notLineBulletSpeed.x >> notLineBulletSpeed.y >> notLineBulletAccel.x >> notLineBulletAccel.y;
				public_bullet.notLineBulletSpeed[i] = notLineBulletSpeed;
				public_bullet.notLineBulletAccel[i] = notLineBulletAccel;
			}
			*file >> trash >> bulletColor >> trash;
			public_bullet.bulletColor[i] = bulletColor;

			i++;
		} while (trash != "}");
	}

	int read_time(ifstream *file) {
		int beats4, beat, beat1_2, beat1_3, beat1_4, beat1_6, beat1_8, beat1_16, beat1_32;
		*file >> beats4 >> beat >> beat1_2 >> beat1_3 >> beat1_4 >> beat1_6 >> beat1_8 >> beat1_16 >> beat1_32;
		return beats4 * 128 + beat * 32 + beat1_2 * 16 + beat1_3 * 11 + beat1_4 * 8 + beat1_6 * 5 + beat1_8 * 4 + beat1_16 * 2 + beat1_32;
	}
};

