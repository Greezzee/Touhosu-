#pragma once
#include <fstream>
using namespace std;
struct plan_exemplar {
	// начало b_ означает время в 32 долях бита
	int b_start_time = -1, b_end_time = -1, b_preparing_time = -1, types_of_bullets;
	string type = "", shoot_type = "";
	//1-16 color ID, 0 for random
	int bulletColor = 0;
	//'a' for absolute
	//'r' for relative
	//'p' for angle to player from gun
	//'s' for angle to player from spawn pos
	char angle_modificator = 'a';
	char start_moving_modificator = 'a';
	sf::Vector2f startMovingCoords;
	long double end_pos_x = -1, end_pos_y = -1, end_angle = -1;
	long double shoot_size = -1;
	long double shoot_speed = 0, shoot_speed_x = 0, shoot_speed_y = 0, shoot_acceleration = 0, shoot_acceleration_x = 0, shoot_acceleration_y = 0, shoot_angle = 0;
	long double gun_speed_x = 0, gun_speed_y = 0;
	bool is_rotate_direction_clockwise = false;
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
			return plan1.b_start_time < plan2.b_start_time;
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
						new_plan.b_start_time += deltaTime;
						new_plan.b_end_time += deltaTime;
						new_plan.b_preparing_time += deltaTime;
					}
			}
		} while (command_type != "end");
	}

	plan_exemplar createNewPlanExemplar(ifstream *file) {
		plan_exemplar new_plan;
		string trash;
		*file >> trash;
		new_plan.b_start_time = read_time(file);
		*file >> trash;
		*file >> new_plan.type;
		if (new_plan.type == "set") new_plan = readSet(file, new_plan);
		else if (new_plan.type == "del");
		else if (new_plan.type == "move") new_plan = readMove(file, new_plan);
		else if (new_plan.type == "rotate") new_plan = readRotate(file, new_plan);
		else if (new_plan.type == "laser_shoot") new_plan = readLaserShoot(file, new_plan);
		else if (new_plan.type == "bullet_shoot") new_plan = readBulletShoot(file, new_plan);
		return new_plan;
	}

	plan_exemplar readSet(ifstream *file, plan_exemplar new_plan) {
		*file >> new_plan.end_pos_x >> new_plan.end_pos_y >> new_plan.shoot_angle;
		return new_plan;
	}
	plan_exemplar readMove(ifstream *file, plan_exemplar new_plan) {
		string trash;
		*file >> trash;
		new_plan.b_end_time = read_time(file);
		*file >> trash;
		*file >> new_plan.angle_modificator >> new_plan.end_pos_x >> new_plan.end_pos_y;
		return new_plan;
	}
	plan_exemplar readRotate(ifstream *file, plan_exemplar new_plan) {
		string trash;
		*file >> trash;
		new_plan.b_end_time = read_time(file);
		*file >> trash;
		*file >> new_plan.angle_modificator >> new_plan.end_angle >> new_plan.is_rotate_direction_clockwise;
		return new_plan;
	}
	plan_exemplar readLaserShoot(ifstream *file, plan_exemplar new_plan) {
		string trash;
		*file >> trash >> new_plan.start_moving_modificator >> new_plan.startMovingCoords.x >> new_plan.startMovingCoords.y;
		*file >> trash;
		new_plan.b_preparing_time = read_time(file);
		*file >> trash;
		new_plan.b_end_time = read_time(file);
		*file >> trash;
		*file >> new_plan.angle_modificator >> new_plan.shoot_angle >> new_plan.shoot_size;
		return new_plan;
	}
	plan_exemplar readBulletShoot(ifstream *file, plan_exemplar new_plan) {
		string trash;
		string public_or_local;
		*file >> trash >> new_plan.start_moving_modificator >> new_plan.startMovingCoords.x >> new_plan.startMovingCoords.y >> public_or_local;
		if (public_or_local == "lb") {
			*file >> new_plan.shoot_type >> new_plan.shoot_size >> new_plan.angle_modificator >> new_plan.shoot_angle;
			if (new_plan.shoot_type == "line") *file >> new_plan.shoot_speed >> new_plan.shoot_acceleration;
			else if (new_plan.shoot_type == "not_line_speed") *file >> new_plan.shoot_speed_x >> new_plan.shoot_speed_y >> new_plan.shoot_acceleration;
			else if (new_plan.shoot_type == "not_line_accel") *file >> new_plan.shoot_speed >> new_plan.shoot_acceleration_x >> new_plan.shoot_acceleration_y;
			else if (new_plan.shoot_type == "not_line") *file >> new_plan.shoot_speed_x >> new_plan.shoot_speed_y >> new_plan.shoot_acceleration_x >> new_plan.shoot_acceleration_y;
			*file >> trash >> new_plan.bulletColor;
		}
		else if (public_or_local == "pb") {
			new_plan.shoot_size = public_bullet.shoot_size;
			new_plan.shoot_angle = public_bullet.shoot_angle;
			new_plan.shoot_speed = public_bullet.shoot_speed;
			new_plan.shoot_speed_x = public_bullet.shoot_speed_x;
			new_plan.shoot_speed_y = public_bullet.shoot_speed_y;
			new_plan.shoot_acceleration = public_bullet.shoot_acceleration;
			new_plan.shoot_acceleration_x = public_bullet.shoot_acceleration_x;
			new_plan.shoot_acceleration_y = public_bullet.shoot_acceleration_y;
			new_plan.angle_modificator = public_bullet.angle_modificator;
			new_plan.bulletColor = public_bullet.bulletColor;
			new_plan.start_moving_modificator = public_bullet.start_moving_modificator;
			new_plan.startMovingCoords = public_bullet.startMovingCoords;
		}
		return new_plan;
	}

	void setNewPublicBullet(ifstream *file) {
		string trash;
		*file >> trash;
		plan_exemplar n;
		public_bullet = n;
		*file >> trash >> public_bullet.start_moving_modificator >> public_bullet.startMovingCoords.x >> public_bullet.startMovingCoords.y >> public_bullet.shoot_type >> public_bullet.shoot_size >> public_bullet.angle_modificator >> public_bullet.shoot_angle;
		if (public_bullet.shoot_type == "line") *file >> public_bullet.shoot_speed >> public_bullet.shoot_acceleration;
		else if (public_bullet.shoot_type == "not_line_speed") *file >> public_bullet.shoot_speed_x >> public_bullet.shoot_speed_y >> public_bullet.shoot_acceleration;
		else if (public_bullet.shoot_type == "not_line_accel") *file >> public_bullet.shoot_speed >> public_bullet.shoot_acceleration_x >> public_bullet.shoot_acceleration_y;
		else if (public_bullet.shoot_type == "not_line") *file >> public_bullet.shoot_speed_x >> public_bullet.shoot_speed_y >> public_bullet.shoot_acceleration_x >> public_bullet.shoot_acceleration_y;
		*file >> trash >> public_bullet.bulletColor;
	}

	int read_time(ifstream *file) {
		int beats4, beat, beat1_2, beat1_3, beat1_4, beat1_6, beat1_8, beat1_16, beat1_32;
		*file >> beats4 >> beat >> beat1_2 >> beat1_3 >> beat1_4 >> beat1_6 >> beat1_8 >> beat1_16 >> beat1_32;
		return beats4 * 128 + beat * 32 + beat1_2 * 16 + beat1_3 * 11 + beat1_4 * 8 + beat1_6 * 5 + beat1_8 * 4 + beat1_16 * 2 + beat1_32;
	}
};

