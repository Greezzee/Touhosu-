#pragma once
#include <fstream>
using namespace std;

struct plan_exemplar {
	//a - absolute music time
	//r - relative music time (from spawn time to action time)
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
			
			*file >> trash;

			do {
				if (new_plan.bulletMovmentType.size() != 0) {
					char timeType;
					*file >> timeType;
					new_plan.timeType.push_back(timeType);
					new_plan.startTime.push_back(read_time(file));
					*file >> trash;
				}
				string bulletMovmentType;
				char angleType;
				double bulletSize, shootAngle;
				int bulletColor;
				*file >> bulletMovmentType >> bulletSize >> angleType >> shootAngle;
				new_plan.bulletMovmentType.push_back(bulletMovmentType);
				new_plan.bulletSize.push_back(bulletSize);
				new_plan.angleType.push_back(angleType);
				new_plan.shootAngle.push_back(shootAngle);

				if (bulletMovmentType == "line") {
					double lineBulletSpeed, lineBulletAccel;
					*file >> lineBulletSpeed >> lineBulletAccel;
					new_plan.lineBulletSpeed.push_back(lineBulletSpeed);
					new_plan.lineBulletAccel.push_back(lineBulletAccel);
				}
				else if (bulletMovmentType == "not_line_speed") {
					sf::Vector2f notLineBulletSpeed;
					double lineBulletAccel;
					*file >> notLineBulletSpeed.x >> notLineBulletSpeed.y >> lineBulletAccel;
					new_plan.notLineBulletSpeed.push_back(notLineBulletSpeed);
					new_plan.lineBulletAccel.push_back(lineBulletAccel);
				}
				else if (bulletMovmentType == "not_line_accel") {
					sf::Vector2f notLineBulletAccel;
					double lineBulletSpeed;
					*file >> lineBulletSpeed >> notLineBulletAccel.x >> notLineBulletAccel.y;
				}
				else if (bulletMovmentType == "not_line") {
					sf::Vector2f notLineBulletAccel, notLineBulletSpeed;
					*file >> notLineBulletSpeed.x >> notLineBulletSpeed.y >> notLineBulletAccel.x >> notLineBulletAccel.y;
					new_plan.notLineBulletSpeed.push_back(notLineBulletSpeed);
					new_plan.notLineBulletAccel.push_back(notLineBulletAccel);
				}
				*file >> trash >> bulletColor >> trash;
				new_plan.bulletColor.push_back(bulletColor);
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

		do {
			if (public_bullet.bulletMovmentType.size() != 0) {
				char timeType;
				*file >> timeType;
				public_bullet.timeType.push_back(timeType);
				public_bullet.startTime.push_back(read_time(file));
			}
			string bulletMovmentType;
			char angleType;
			double bulletSize, shootAngle;
			int bulletColor;
			*file >> bulletMovmentType >> bulletSize >> angleType >> shootAngle;
			public_bullet.bulletMovmentType.push_back(bulletMovmentType);
			public_bullet.bulletSize.push_back(bulletSize);
			public_bullet.angleType.push_back(angleType);
			public_bullet.shootAngle.push_back(shootAngle);

			if (bulletMovmentType == "line") {
				double lineBulletSpeed, lineBulletAccel;
				*file >> lineBulletSpeed >> lineBulletAccel;
				public_bullet.lineBulletSpeed.push_back(lineBulletSpeed);
				public_bullet.lineBulletAccel.push_back(lineBulletAccel);
			}
			else if (bulletMovmentType == "not_line_speed") {
				sf::Vector2f notLineBulletSpeed;
				double lineBulletAccel;
				*file >> notLineBulletSpeed.x >> notLineBulletSpeed.y >> lineBulletAccel;
				public_bullet.notLineBulletSpeed.push_back(notLineBulletSpeed);
				public_bullet.lineBulletAccel.push_back(lineBulletAccel);
			}
			else if (bulletMovmentType == "not_line_accel") {
				sf::Vector2f notLineBulletAccel;
				double lineBulletSpeed;
				*file >> lineBulletSpeed >> notLineBulletAccel.x >> notLineBulletAccel.y;
				public_bullet.lineBulletSpeed.push_back(lineBulletSpeed);
				public_bullet.notLineBulletAccel.push_back(notLineBulletAccel);
			}
			else if (bulletMovmentType == "not_line") {
				sf::Vector2f notLineBulletAccel, notLineBulletSpeed;
				*file >> notLineBulletSpeed.x >> notLineBulletSpeed.y >> notLineBulletAccel.x >> notLineBulletAccel.y;
				public_bullet.notLineBulletSpeed.push_back(notLineBulletSpeed);
				public_bullet.notLineBulletAccel.push_back(notLineBulletAccel);
			}
			*file >> trash >> bulletColor >> trash;
			public_bullet.bulletColor.push_back(bulletColor);
		} while (trash != "}");
	}

	int read_time(ifstream *file) {
		int beats4, beat, beat1_2, beat1_3, beat1_4, beat1_6, beat1_8, beat1_16, beat1_32;
		*file >> beats4 >> beat >> beat1_2 >> beat1_3 >> beat1_4 >> beat1_6 >> beat1_8 >> beat1_16 >> beat1_32;
		return beats4 * 128 + beat * 32 + beat1_2 * 16 + beat1_3 * 11 + beat1_4 * 8 + beat1_6 * 5 + beat1_8 * 4 + beat1_16 * 2 + beat1_32;
	}
};

