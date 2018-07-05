#pragma once
using namespace sf;
class gun
{
public:
	gun() {}
	void set_gun(long double bpm, Sprite g, gun_plan mp) {
		plan = mp;
		next_action = plan.get_plan();
		self_sprite = g;
		self_sprite.setOrigin(0, 256);
		self_sprite.setPosition(0, 0);
		self_sprite.setScale(0.125 * SCREEN_H / GAMEBOARD_H, 0.125 * SCREEN_H / GAMEBOARD_H);
		x_coord = 0;
		y_coord = 0;
		shoot_angle = 0;
		example.loadFromFile(bulletsAndHitboxesFile);
		l_example.loadFromFile(laserFile);
		song_bpm = bpm;
		current_beat = 0;
		timer = 0;
		is_visible = false;
		is_laser_shoot = false;
		current_actions.resize(0);
	}

	void update(RenderWindow *window, long double time, std::vector<bullet> *all_bullets, std::vector<laser> *all_lasers, player *target) {
		timer += time;
		int numberOfBeatThisTurn = 0;
		bool new_tick = false;
		if (timer >= timePerBeat) {
			while (timer > time) {
				timer -= timePerBeat;
				current_beat++;
				numberOfBeatThisTurn++;
				new_tick = true;
			}
		}

		for (int j = numberOfBeatThisTurn - 1; j >= 0; j--) {
			while (true) {
				if (current_beat - j == next_action.b_start_time) {
					start_action();
					next_action = plan.get_plan();
				}
				if (current_beat - j != next_action.b_start_time) break;
			}
		}

		for (int i = 0; i < current_actions.size(); i++) {
			if (action(current_actions[i], new_tick, all_bullets, all_lasers, target)) {
				current_actions.erase(current_actions.begin() + i);
				i--;
			}
		}
		if (is_visible) window->draw(self_sprite);
	}

private:
	long double x_coord, y_coord, shoot_angle, timer, angle_speed, song_bpm;
	int current_beat;
	bool is_visible, is_laser_shoot;
	Texture self_text;
	Sprite self_sprite;
	Texture example, l_example;
	plan_exemplar next_action;
	vector<plan_exemplar> current_actions;
	gun_plan plan;
	void start_action() {
		if (next_action.type == "move") {
			double move_distance_x, move_distance_y;
			int delta = next_action.b_end_time - next_action.b_start_time;
			if (next_action.angle_modificator == 'a') {
				move_distance_x = next_action.end_pos_x - x_coord;
				move_distance_y = next_action.end_pos_y - y_coord;
			}
			else if (next_action.angle_modificator == 'r') {
				move_distance_x = next_action.end_pos_x;
				move_distance_y = next_action.end_pos_y;
			}
			next_action.gun_speed_x = move_distance_x / delta;
			next_action.gun_speed_y = move_distance_y / delta;
		}
		if (next_action.type == "rotate") {
			int delta = next_action.b_end_time - next_action.b_start_time;
			double rotate;
			if (next_action.angle_modificator == 'r') {
				rotate = next_action.end_angle;
				if (next_action.is_rotate_direction_clockwise) rotate *= -1;
				next_action.end_angle = shoot_angle + rotate;
				next_action.end_angle = LeadAngleToTrigonometric(next_action.end_angle);
			}
			else if (next_action.angle_modificator == 'a') {
				double GunAngleInNewCoordinatesSystem = shoot_angle + 360 - next_action.end_angle;
				GunAngleInNewCoordinatesSystem = LeadAngleToTrigonometric(GunAngleInNewCoordinatesSystem);
				if (next_action.is_rotate_direction_clockwise) rotate = -GunAngleInNewCoordinatesSystem;
				else rotate = 360 - GunAngleInNewCoordinatesSystem;
			}
			if (next_action.angle_modificator != 'p') next_action.gun_speed_x = rotate / delta;
		}
		current_actions.push_back(next_action);
	}
	bool action(plan_exemplar current_action, bool new_tick, std::vector<bullet> *all_bullets, std::vector<laser> *all_lasers, player *target) {
		if (current_action.type == "set") {
			is_visible = true;
			self_sprite.setPosition(current_action.end_pos_x * SCREEN_H / GAMEBOARD_H, current_action.end_pos_y * SCREEN_H / GAMEBOARD_H);
			self_sprite.setRotation(-current_action.shoot_angle);
			shoot_angle = current_action.shoot_angle;
			x_coord = current_action.end_pos_x;
			y_coord = current_action.end_pos_y;
			return true;
		}
		else if (current_action.type == "del") {
			is_visible = false;
			return true;
		}
		else if (current_action.type == "move" && new_tick) {
			self_sprite.move(current_action.gun_speed_x * SCREEN_H / GAMEBOARD_H, current_action.gun_speed_y * SCREEN_H / GAMEBOARD_H);
			x_coord += current_action.gun_speed_x;
			y_coord += current_action.gun_speed_y;
			if (current_action.b_end_time == current_beat) return true;
			else return false;
		}
		else if (current_action.type == "laser_shoot") {
			Sprite s;
			s.setTexture(l_example);
			laser actual_laser;
			double laserAngle;
			if (current_action.angle_modificator == 'a') laserAngle = current_action.shoot_angle;
			else if (current_action.angle_modificator == 'r') laserAngle = LeadAngleToTrigonometric(shoot_angle + current_action.shoot_angle);
			else if (current_action.angle_modificator == 'p') laserAngle = LeadAngleToTrigonometric(atan2(y_coord - target->playerCoords.y, target->playerCoords.x - x_coord) * 180 / PI) + current_action.shoot_angle;
			else if (current_action.angle_modificator == 's') {
				if (current_action.start_moving_modificator == 'r') laserAngle = LeadAngleToTrigonometric(atan2(y_coord + current_action.startMovingCoords.y - target->playerCoords.y, target->playerCoords.x - current_action.startMovingCoords.x - x_coord) * 180 / PI) + current_action.shoot_angle;
				else laserAngle = LeadAngleToTrigonometric(atan2(current_action.startMovingCoords.y - target->playerCoords.y, target->playerCoords.x - current_action.startMovingCoords.x) * 180 / PI) + current_action.shoot_angle;
			}
			if (current_action.start_moving_modificator == 'a') actual_laser.create(current_action.startMovingCoords.x, current_action.startMovingCoords.y, laserAngle, current_action.shoot_size, &s);
			else actual_laser.create(current_action.startMovingCoords.x + x_coord, current_action.startMovingCoords.y + y_coord, laserAngle, current_action.shoot_size, &s);
			if (current_beat >= current_action.b_preparing_time) actual_laser.activate();
			else if (current_action.start_moving_modificator == 'r') actual_laser.activate_animation(current_beat - current_action.b_start_time, current_action.b_preparing_time - current_action.b_start_time);
			all_lasers->push_back(actual_laser);
			if (current_action.b_end_time == current_beat) return true;
			else return false;
		}

		else if (current_action.type == "bullet_shoot") {
			Sprite s;
			s.setTexture(example);
			s.setTextureRect(IntRect(64, 96, 32, 32));
			s.setOrigin(16, 16);
			bullet new_bullet;
			double bulletAngle;
			if (current_action.angle_modificator == 'a') bulletAngle = current_action.shoot_angle;
			else if (current_action.angle_modificator == 'r') bulletAngle = LeadAngleToTrigonometric(shoot_angle + current_action.shoot_angle);
			else if (current_action.angle_modificator == 'p') bulletAngle = LeadAngleToTrigonometric(atan2(y_coord - target->playerCoords.y, target->playerCoords.x - x_coord) * 180 / PI) + current_action.shoot_angle;
			else if (current_action.angle_modificator == 's') {
				if (current_action.start_moving_modificator == 'r') bulletAngle = LeadAngleToTrigonometric(atan2(y_coord + current_action.startMovingCoords.y - target->playerCoords.y, target->playerCoords.x - current_action.startMovingCoords.x - x_coord) * 180 / PI) + current_action.shoot_angle;
				else bulletAngle = LeadAngleToTrigonometric(atan2(current_action.startMovingCoords.y - target->playerCoords.y, target->playerCoords.x - current_action.startMovingCoords.x) * 180 / PI) + current_action.shoot_angle;
			}
			if (current_action.start_moving_modificator == 'a') new_bullet.create(current_action.startMovingCoords.x, current_action.startMovingCoords.y, bulletAngle, &current_action, &s);
			else if (current_action.start_moving_modificator == 'r') new_bullet.create(current_action.startMovingCoords.x + x_coord, current_action.startMovingCoords.y + y_coord, bulletAngle, &current_action, &s);
			all_bullets->push_back(new_bullet);
			return true;
		}
		else if (current_action.type == "rotate" && new_tick) {
			if (current_action.angle_modificator == 'a' || current_action.angle_modificator == 'r') {
				self_sprite.rotate(-current_action.gun_speed_x);
				shoot_angle += current_action.gun_speed_x;
				shoot_angle = LeadAngleToTrigonometric(shoot_angle);
				if (current_action.b_end_time == current_beat) {
					shoot_angle = current_action.end_angle;
					self_sprite.setRotation(-current_action.end_angle);
					return true;
				}
				else return false;
			}
			else {
				shoot_angle = LeadAngleToTrigonometric(atan2(y_coord - target->playerCoords.y, target->playerCoords.x - x_coord) * 180 / PI);
				self_sprite.setRotation(-shoot_angle);
				if (current_action.b_end_time == current_beat) return true;
				else return false;
			}
		}
		else return false;
	}


	double LeadAngleToTrigonometric(double angle) {
		if (angle >= 360) angle -= 360;
		else if (angle < 0) angle += 360;
		return angle;
	}
};


