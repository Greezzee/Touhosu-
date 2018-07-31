#pragma once
#include <fstream>
using namespace std;

struct bullet_exemplar {

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
	vector<double> lineBulletSpeed, lineBulletAccel, bulletSize, bulletSpeedAngle, bulletAccelAngle;

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

struct plan_exemplar {
	bullet_exemplar bulletInfo;
	int startTime = -1;
	int endTime = -1, laserPreparingEndTime = -1;
	char angleType = 'a';
	double gunEndAngle = 0, angleSpeed = 0, shootAngle = 0;
	sf::Vector2f startMovingCoords, endMovingCoords, gunSpeed;
	std::string commandType;
	bool isRotateClockwise = false;
	double laserSize = 0;
	//'a' for absolute
	//'r' for relative without gunAngle
	//'s' for relative with gunAngle
	char startMovingType = 'a';
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
			return plan1.startTime < plan2.startTime;
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
	bullet_exemplar public_bullet;
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
						new_plan.startTime += deltaTime;
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

	plan_exemplar readSet(ifstream *file, plan_exemplar new_plan) {
		*file >> new_plan.endMovingCoords.x >> new_plan.endMovingCoords.y >> new_plan.shootAngle;
		return new_plan;
	}
	plan_exemplar readMove(ifstream *file, plan_exemplar new_plan) {
		string trash;
		*file >> trash;
		new_plan.endTime = read_time(file);
		*file >> trash;
		*file >> new_plan.angleType >> new_plan.endMovingCoords.x >> new_plan.endMovingCoords.y;
		return new_plan;
	}
	plan_exemplar readRotate(ifstream *file, plan_exemplar new_plan) {
		string trash;
		*file >> trash;
		new_plan.endTime = read_time(file);
		*file >> trash;
		*file >> new_plan.angleType >> new_plan.gunEndAngle >> new_plan.isRotateClockwise;
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
		*file >> new_plan.angleType >> new_plan.shootAngle >> new_plan.laserSize;
		return new_plan;
	}
	plan_exemplar readBulletShoot(ifstream *file, plan_exemplar new_plan) {
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
				double bulletSize, bulletAccelAngle, bulletSpeedAngle, lineBulletSpeed, lineBulletAccel;
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

			new_plan.bulletInfo.timeType = public_bullet.timeType;
			new_plan.bulletInfo.startTime = public_bullet.startTime;

			new_plan.bulletInfo.bulletActionWithWalls = public_bullet.bulletActionWithWalls;
			new_plan.bulletInfo.bulletSize = public_bullet.bulletSize;

			new_plan.bulletInfo.accelAngleType = public_bullet.accelAngleType;
			new_plan.bulletInfo.accelOffsetCoord = public_bullet.accelOffsetCoord;
			new_plan.bulletInfo.bulletAccelAngle = public_bullet.bulletAccelAngle;

			new_plan.bulletInfo.speedAngleType = public_bullet.speedAngleType;
			new_plan.bulletInfo.speedOffsetCoord = public_bullet.speedOffsetCoord;
			new_plan.bulletInfo.bulletSpeedAngle = public_bullet.bulletSpeedAngle;
			new_plan.bulletInfo.speedChangeType = public_bullet.speedChangeType;

			new_plan.bulletInfo.lineBulletSpeed = public_bullet.lineBulletSpeed;
			new_plan.bulletInfo.lineBulletAccel = public_bullet.lineBulletAccel;

			new_plan.bulletInfo.bulletColor = public_bullet.bulletColor;
			new_plan.bulletInfo.bulletSkin = public_bullet.bulletSkin;
		}
		return new_plan;
	}

	void setNewPublicBullet(ifstream *file) {
		string trash;
		bullet_exemplar n;
		public_bullet = n;
		*file >> trash;

		public_bullet.startTime.resize(0);
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
			string accelAngleType, speedAngleType, bulletSkin;
			double bulletSize, bulletAccelAngle, bulletSpeedAngle, lineBulletSpeed, lineBulletAccel;
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

		
			public_bullet.startTime.push_back(startTime);
			

			public_bullet.timeType.push_back(timeType);
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
			public_bullet.bulletSkin.push_back(bulletSkin);

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

