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
		timer = 0;
		is_visible = false;
		is_laser_shoot = false;
		current_actions.resize(0);
	}

	void update(RenderWindow *window, float time, bulletManager *manager, player *target, planner *GlobalMapPlan) {
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
			if (action(*i, manager, target, time)) i = current_actions.erase(i);
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
	bool action(gunPlanExemplar current_action, bulletManager *manager, player *target, float time) {
		if (current_action.commandType == "set") {
			is_visible = true;
			self_sprite.setPosition(convertForGraphic(current_action.endMovingCoords.x), convertForGraphic(current_action.endMovingCoords.y));
			self_sprite.setRotation(-current_action.gunAngle);
			shoot_angle = current_action.gunAngle;
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
			manager->createLaser(&current_action, shoot_angle, coords, target);
			if (current_action.endTime <= current_beat) return true;
			else return false;
		}

		else if (current_action.commandType == "bullet_shoot") {
			manager->createBullet(&current_action, shoot_angle, coords, target);
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


