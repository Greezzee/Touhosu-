#pragma once
using namespace sf;
class gun
{
public:
	void set_gun(Sprite g, planner *GlobalMapPlan, int id) {
		isActionsEnd = false;
		selfID = id;

		pair<bool, gunPlanExemplar> returned = GlobalMapPlan->getGunPlan(selfID);
		if (returned.first) next_action = returned.second;
		else isActionsEnd = true;
			
		self_sprite = g;
		self_sprite.setOrigin(0, 256);
		self_sprite.setPosition(0, 0);
		self_sprite.setScale(convertForGraphic(0.125f), convertForGraphic(0.125f));
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

	void update(RenderWindow *window, float time, std::list<bullet> *all_bullets, std::list<laser> *all_lasers, player *target, planner *GlobalMapPlan) {
		actionsThisFrame = 0;
		if (!isActionsEnd)
		{
			pair<bool, gunPlanExemplar> returned;
			for (int j = numberOfBeatThisTurn - 1; j >= 0; j--) {
				while (true) {
					if (current_beat - j >= next_action.startTime) {
						start_action();
						returned = GlobalMapPlan->getGunPlan(selfID);
						if (returned.first) {
							next_action = returned.second;
							actionsThisFrame++;
						}
						else {
							isActionsEnd = true;
							break;
						}
						if (actionsThisFrame >= 36) break;
					}
					if (current_beat - j < next_action.startTime) break;
				}
			}
		}
		
		for (list<gunPlanExemplar>::iterator i = current_actions.begin(); i != current_actions.end();) {
			if (action(*i, all_bullets, all_lasers, target, time)) i = current_actions.erase(i);
			else i++;
		}
		
		if (is_visible) window->draw(self_sprite);
	}

private:
	Vector2f coords;
	float shoot_angle, angle_speed;
	bool is_visible, is_laser_shoot, isActionsEnd;
	Texture self_text;
	Sprite self_sprite;
	Texture example, l_example;
	gunPlanExemplar next_action;
	list<gunPlanExemplar> current_actions;
	int selfID, actionsThisFrame;
	void start_action() {
		if (next_action.commandType == "move") {
			float move_distance_x, move_distance_y;
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
			float rotate;
			if (next_action.angleType == 'r') {
				rotate = next_action.gunEndAngle;
				if (next_action.isRotateClockwise) rotate *= -1;
				next_action.gunEndAngle = shoot_angle + rotate;
				next_action.gunEndAngle = LeadAngleToTrigonometric(next_action.gunEndAngle);
			}
			else if (next_action.angleType == 'a') {
				float GunAngleInNewCoordinatesSystem = shoot_angle + 360 - next_action.gunEndAngle;
				GunAngleInNewCoordinatesSystem = LeadAngleToTrigonometric(GunAngleInNewCoordinatesSystem);
				if (next_action.isRotateClockwise) rotate = -GunAngleInNewCoordinatesSystem;
				else rotate = 360 - GunAngleInNewCoordinatesSystem;
			}
			if (next_action.angleType != 'p') next_action.angleSpeed = rotate / delta;
		}
		current_actions.push_back(next_action);
	}
	bool action(gunPlanExemplar current_action, std::list<bullet> *all_bullets, std::list<laser> *all_lasers, player *target, float time) {
		if (current_action.commandType == "set") {
			is_visible = true;
			self_sprite.setPosition(convertForGraphic(current_action.endMovingCoords.x), convertForGraphic(current_action.endMovingCoords.y));
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
				self_sprite.setPosition(convertForGraphic(coords.x), convertForGraphic(coords.y));
			}
			
			else self_sprite.move(convertForGraphic(current_action.gunSpeed.x) / timePerBeat * time, convertForGraphic(current_action.gunSpeed.y) / timePerBeat * time);

			if (current_action.endTime <= current_beat) return true;
			else return false;
			
		}
		else if (current_action.commandType == "laser_shoot") {
			Sprite s;
			s.setTexture(l_example);
			laser actual_laser;
			float laserAngle;
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
		else return true;
	}
};


