#pragma once
using namespace sf;
class gun
{
public:
	gun() {}
	void set_gun(Sprite g, gun_plan mp) {
		plan = mp;
		next_action = plan.get_plan();
		self_sprite = g;
		self_sprite.setOrigin(0, 256);
		self_sprite.setPosition(0, 0);
		self_sprite.setScale(0.125 * SCREEN_H / GAMEBOARD_H, 0.125 * SCREEN_H / GAMEBOARD_H);
		coords.x = 0;
		coords.y = 0;
		shoot_angle = 0;
		example.loadFromFile(bulletsAndHitboxesFile);
		l_example.loadFromFile(laserFile);
		timer = 0;
		is_visible = false;
		is_laser_shoot = false;
		current_actions.resize(0);
	}

	void update(RenderWindow *window, double time, std::vector<bullet> *all_bullets, std::vector<laser> *all_lasers, player *target) {

		for (int j = numberOfBeatThisTurn - 1; j >= 0; j--) {
			while (true) {
				if (current_beat - j == next_action.startTime) {
					start_action();
					next_action = plan.get_plan();
				}
				if (current_beat - j != next_action.startTime) break;
			}
		}

		for (int i = 0; i < current_actions.size(); i++) {
			if (action(current_actions[i], all_bullets, all_lasers, target, time)) {
				current_actions.erase(current_actions.begin() + i);
				i--;
			}
		}
		if (is_visible) window->draw(self_sprite);
	}

private:
	Vector2f coords;
	double shoot_angle, angle_speed;
	bool is_visible, is_laser_shoot;
	Texture self_text;
	Sprite self_sprite;
	Texture example, l_example;
	plan_exemplar next_action;
	vector<plan_exemplar> current_actions;
	gun_plan plan;
	void start_action() {
		if (next_action.commandType == "move") {
			double move_distance_x, move_distance_y;
			int delta = next_action.endTime - next_action.startTime;
			if (next_action.angleType == 'a') {
				move_distance_x = next_action.endMovingCoords.x - coords.x;
				move_distance_y = next_action.endMovingCoords.y - coords.y;
			}
			else if (next_action.angleType == 'r') {
				move_distance_x = next_action.endMovingCoords.x;
				move_distance_y = next_action.endMovingCoords.y;
			}
			next_action.gunSpeed.x = move_distance_x / delta;
			next_action.gunSpeed.y = move_distance_y / delta;
		}
		if (next_action.commandType == "rotate") {
			int delta = next_action.endTime - next_action.startTime;
			double rotate;
			if (next_action.angleType == 'r') {
				rotate = next_action.gunEndAngle;
				if (next_action.isRotateClockwise) rotate *= -1;
				next_action.gunEndAngle = shoot_angle + rotate;
				next_action.gunEndAngle = LeadAngleToTrigonometric(next_action.gunEndAngle);
			}
			else if (next_action.angleType == 'a') {
				double GunAngleInNewCoordinatesSystem = shoot_angle + 360 - next_action.gunEndAngle;
				GunAngleInNewCoordinatesSystem = LeadAngleToTrigonometric(GunAngleInNewCoordinatesSystem);
				if (next_action.isRotateClockwise) rotate = -GunAngleInNewCoordinatesSystem;
				else rotate = 360 - GunAngleInNewCoordinatesSystem;
			}
			if (next_action.angleType != 'p') next_action.angleSpeed = rotate / delta;
		}
		current_actions.push_back(next_action);
	}
	bool action(plan_exemplar current_action, std::vector<bullet> *all_bullets, std::vector<laser> *all_lasers, player *target, double time) {
		if (current_action.commandType == "set") {
			is_visible = true;
			self_sprite.setPosition(current_action.endMovingCoords.x * SCREEN_H / GAMEBOARD_H, current_action.endMovingCoords.y * SCREEN_H / GAMEBOARD_H);
			self_sprite.setRotation(-current_action.shootAngle);
			shoot_angle = current_action.shootAngle;
			coords.x = current_action.endMovingCoords.x;
			coords.y = current_action.endMovingCoords.y;
			return true;
		}
		else if (current_action.commandType == "del") {
			is_visible = false;
			return true;
		}
		else if (current_action.commandType == "move") {
			if (newTick) {
				coords.x += current_action.gunSpeed.x;
				coords.y += current_action.gunSpeed.y;
				self_sprite.setPosition(coords.x * (float)SCREEN_H / (float)GAMEBOARD_H, coords.y * (float)SCREEN_H / (float)GAMEBOARD_H);
			}
			
			else self_sprite.move(current_action.gunSpeed.x / timePerBeat * (float)SCREEN_H / (float)GAMEBOARD_H * time, current_action.gunSpeed.y / timePerBeat * (float)SCREEN_H / (float)GAMEBOARD_H * time);

			if (current_action.endTime <= current_beat) return true;
			else return false;
			
		}
		else if (current_action.commandType == "laser_shoot") {
			Sprite s;
			s.setTexture(l_example);
			laser actual_laser;
			double laserAngle;
			if (current_action.angleType == 'a') laserAngle = current_action.shootAngle;
			else if (current_action.angleType == 'r') laserAngle = LeadAngleToTrigonometric(shoot_angle + current_action.shootAngle);
			else if (current_action.angleType == 'p') laserAngle = LeadAngleToTrigonometric(atan2(coords.y - target->playerCoords.y, target->playerCoords.x - coords.x) * 180 / PI) + current_action.shootAngle;
			else if (current_action.angleType == 's') {
				if (current_action.startMovingType == 'r') laserAngle = LeadAngleToTrigonometric(atan2(coords.y + current_action.startMovingCoords.y - target->playerCoords.y, target->playerCoords.x - current_action.startMovingCoords.x - coords.x) * 180 / PI) + current_action.shootAngle;
				else laserAngle = LeadAngleToTrigonometric(atan2(current_action.startMovingCoords.y - target->playerCoords.y, target->playerCoords.x - current_action.startMovingCoords.x) * 180 / PI) + current_action.shootAngle;
			}
			if (current_action.startMovingType == 'a') actual_laser.create(current_action.startMovingCoords.x, current_action.startMovingCoords.y, laserAngle, current_action.laserSize, &s);
			else if (current_action.startMovingType == 'r') actual_laser.create(current_action.startMovingCoords.x + coords.x, current_action.startMovingCoords.y + coords.y, laserAngle, current_action.laserSize, &s);
			else if (current_action.startMovingType == 's') actual_laser.create(current_action.startMovingCoords.x * cos(-shoot_angle / 180 * PI) + current_action.startMovingCoords.y * sin(-shoot_angle / 180 * PI) + coords.x, current_action.startMovingCoords.x * cos(-(shoot_angle + 90) / 180 * PI) + current_action.startMovingCoords.y * sin(-(shoot_angle + 90) / 180 * PI) + coords.y, laserAngle, current_action.laserSize, &s);
			if (current_beat >= current_action.laserPreparingEndTime) actual_laser.activate();
			else actual_laser.activate_animation(current_beat - current_action.startTime, current_action.laserPreparingEndTime - current_action.startTime);
			all_lasers->push_back(actual_laser);
			if (current_action.endTime <= current_beat) return true;
			else return false;
		}

		else if (current_action.commandType == "bullet_shoot") {
			Sprite s;
			s.setTexture(example);
			s.setTextureRect(IntRect(64, 96, 32, 32));
			s.setOrigin(16, 16);
			bullet new_bullet;
			if (current_action.startMovingType == 'a') new_bullet.create(current_action.startMovingCoords.x, current_action.startMovingCoords.y, shoot_angle, coords, &current_action, &s, target);
			else if (current_action.startMovingType == 'r') new_bullet.create(current_action.startMovingCoords.x + coords.x, current_action.startMovingCoords.y + coords.y, shoot_angle, coords, &current_action, &s, target);
			else if (current_action.startMovingType == 's') new_bullet.create(current_action.startMovingCoords.x * cos(-shoot_angle / 180 * PI) + current_action.startMovingCoords.y * sin(-shoot_angle / 180 * PI) + coords.x, current_action.startMovingCoords.x * cos(-(shoot_angle + 90) / 180 * PI) + current_action.startMovingCoords.y * sin(-(shoot_angle + 90) / 180 * PI) + coords.y, shoot_angle, coords, &current_action, &s, target);
			all_bullets->push_back(new_bullet);
			return true;
		}
		else if (current_action.commandType == "rotate") {
			if (newTick) {
				if (current_action.angleType == 'a' || current_action.angleType == 'r') {
					shoot_angle += current_action.angleSpeed;
					shoot_angle = LeadAngleToTrigonometric(shoot_angle);
					self_sprite.setRotation(-shoot_angle);
					if (current_action.endTime <= current_beat) {
						shoot_angle = current_action.gunEndAngle;
						self_sprite.setRotation(-current_action.gunEndAngle);
						return true;
					}
					else return false;
				}
				else {
					shoot_angle = LeadAngleToTrigonometric((atan2(coords.y - target->playerCoords.y, target->playerCoords.x - coords.x) + current_action.gunEndAngle) * 180 / PI);
					self_sprite.setRotation(-shoot_angle);
					if (current_action.endTime <= current_beat) return true;
					else return false;
				}
			}
			else {
				if (current_action.angleType == 'a' || current_action.angleType == 'r') {
					self_sprite.rotate(-current_action.angleSpeed / timePerBeat * time);
					return false;
				}
				else {
					shoot_angle = LeadAngleToTrigonometric((atan2(coords.y - target->playerCoords.y, target->playerCoords.x - coords.x) + current_action.gunEndAngle) * 180 / PI);
					self_sprite.setRotation(-shoot_angle);
					if (current_action.endTime <= current_beat) return true;
					else return false;
				}
			}
		}
		else return false;
	}
};


