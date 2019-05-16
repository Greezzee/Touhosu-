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
		self_sprite.setTextureRect(sf::IntRect(48, 1383, 32, 32));
		self_sprite.setOrigin(16, 16);
		self_sprite.setPosition(0, 0);
		self_sprite.setScale(convertSizeForGraphic(1.f), convertSizeForGraphic(1.f));
		coords.x = 0;
		coords.y = 0;
		shoot_angle = 0;
		timer = 0;
		is_visible = false;
		is_laser_shoot = false;
		spritePos = 0;
		moveType = 'n';
		current_actions.resize(0);
	}

	void update(vector<vector<sf::Sprite>>& bufferSpriteMap, float time, bulletManager *manager, player *target, planner *GlobalMapPlan) {
		spritePos += time * 0.005f;
		if (moveType == 'n') {
			if (spritePos >= 5) spritePos = 0;
			if (spritePos >= 0) {
				self_sprite.setTextureRect(sf::IntRect(48 + 32 * (int)spritePos, 1383 + 32 * textureColorID, 32, 32));
				self_sprite.setScale(convertSizeForGraphic(1.f), convertSizeForGraphic(1.f));
			}
			else self_sprite.setTextureRect(sf::IntRect(208 - 32 * (int)spritePos, 1383 + 32 * textureColorID, 32, 32));
		}
		else if (moveType == 'r') {
			if (spritePos >= 7) spritePos = 3;
			self_sprite.setTextureRect(sf::IntRect(176 + 32 * (int)spritePos, 1383 + 32 * textureColorID, 32, 32));
			self_sprite.setScale(convertSizeForGraphic(-1.f), convertSizeForGraphic(1.f));
		}
		else {
			if (spritePos >= 7) spritePos = 3;
			self_sprite.setTextureRect(sf::IntRect(176 + 32 * (int)spritePos, 1383 + 32 * textureColorID, 32, 32));
		}
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
						if (actionsThisFrame >= 10) break;
					}
					if (current_beat - j < next_action.startTime) break;
				}
			}
		}
		
		for (list<gunPlanExemplar>::iterator i = current_actions.begin(); i != current_actions.end();) {
			if (action(*i, manager, target, time)) i = current_actions.erase(i);
			else i++;
		}
		
		if (is_visible) bufferSpriteMap[2].push_back(self_sprite);
	}

private:
	Vector2f coords;
	float shoot_angle, angle_speed, spritePos;
	bool is_visible, is_laser_shoot, isActionsEnd;
	Texture self_text;
	Sprite self_sprite;
	Texture example, l_example;
	gunPlanExemplar next_action;
	list<gunPlanExemplar> current_actions;
	int selfID, actionsThisFrame, textureColorID;
	//n for standart, l or r for movement
	char moveType;
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
			if (move_distance_x > 0) moveType = 'r';
			else if (move_distance_x < 0) moveType = 'l';
			spritePos = 0;
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
			self_sprite.setPosition(convertPosForGraphic(current_action.endMovingCoords));
			//self_sprite.setRotation(-current_action.gunAngle); Должен вращать "указатель" врага
			shoot_angle = current_action.gunAngle;
			coords.x = current_action.endMovingCoords.x;
			coords.y = current_action.endMovingCoords.y;
			textureColorID = current_action.colorID;
			self_sprite.setTextureRect(sf::IntRect(48, 1383 + 32 * textureColorID, 32, 32));
			spritePos = 0;
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
				self_sprite.setPosition(convertPosForGraphic(coords));
			}
			
			else self_sprite.move(convertPosForGraphic(current_action.gunSpeed).x / timePerBeat * time, convertPosForGraphic(current_action.gunSpeed).y / timePerBeat * time);

			if (current_action.endTime <= current_beat) {
				moveType = 'n';
				spritePos = -2;
				return true;
			}
			else return false;
			
		}
		else if (current_action.commandType == "laser_shoot") {
			manager->addLaser(&current_action, shoot_angle, coords);
			if (current_action.endTime <= current_beat) return true;
			else return false;
		}

		else if (current_action.commandType == "bullet_shoot") {
			manager->addBullet(&current_action, shoot_angle, coords);
			return true;
			
		}
		else if (current_action.commandType == "rotate") {
			if (newTick) {
				if (current_action.angleType == 'a' || current_action.angleType == 'r') {
					shoot_angle += current_action.angleSpeed;
					shoot_angle = LeadAngleToTrigonometric(shoot_angle);
					//self_sprite.setRotation(-shoot_angle); Должен вращать "указатель" врага
					if (current_action.endTime <= current_beat) {
						shoot_angle = current_action.gunEndAngle;
						//self_sprite.setRotation(-current_action.gunEndAngle); Должен вращать "указатель" врага
						return true;
					}
					else return false;
				}
				else {
					shoot_angle = LeadAngleToTrigonometric((atan2(coords.y - target->playerCoords.y, target->playerCoords.x - coords.x) + current_action.gunEndAngle) * 180 / PI);
					//self_sprite.setRotation(-shoot_angle); Должен вращать "указатель" врага
					if (current_action.endTime <= current_beat) return true;
					else return false;
				}
			}
			else {
				if (current_action.angleType == 'a' || current_action.angleType == 'r') {
					//self_sprite.rotate(-current_action.angleSpeed / timePerBeat * time); Должен вращать "указатель" врага
					return false;
				}
				else {
					shoot_angle = LeadAngleToTrigonometric((atan2(coords.y - target->playerCoords.y, target->playerCoords.x - coords.x) + current_action.gunEndAngle) * 180 / PI);
					//self_sprite.setRotation(-shoot_angle); Должен вращать "указатель" врага
					if (current_action.endTime <= current_beat) return true;
					else return false;
				}
			}
		}
		else return true;
	}
};


