#pragma once

class bulletManager
{
public:
	void init() {
		allBullets.resize(0);
		allLasers.resize(0);
		example.loadFromFile(bulletsAndHitboxesFile);
		l_example.loadFromFile(laserFile);
	}
	void updateAll(RenderWindow *window, float time, player *mainPlayer) {
		vector<bulletsFromBullets> bulletsForShoot(0), a(0);
		for (list<bullet>::iterator i = allBullets.begin(); i != allBullets.end();) {
			a = i->update(window, time, mainPlayer);
			bulletsForShoot.insert(bulletsForShoot.end(), a.begin(), a.end());
			if (i->destroyed) i = allBullets.erase(i);
			else i++;
		}
		for (list<laser>::iterator i = allLasers.begin(); i != allLasers.end(); i++) {
			i->update(window, mainPlayer);
		}
		allLasers.clear();
		
		for (int i = 0; i < bulletsForShoot.size(); i++) 
			createBullet(&bulletsForShoot[i].info, bulletsForShoot[i].angle, bulletsForShoot[i].coords, mainPlayer);
	}
	void createBullet(gunPlanExemplar *current_action, float shoot_angle, Vector2f coords, player *target) {
		Sprite s;
		s.setTexture(example);
		s.setTextureRect(IntRect(64, 96, 32, 32));
		s.setOrigin(16, 16);
		bullet new_bullet;
		if (current_action->startMovingType == 'a') new_bullet.create(current_action->startCoords.x, current_action->startCoords.y, shoot_angle, coords, current_action, &s, target);
		else if (current_action->startMovingType == 'r') new_bullet.create(current_action->startCoords.x + coords.x, current_action->startCoords.y + coords.y, shoot_angle, coords, current_action, &s, target);
		else if (current_action->startMovingType == 's') new_bullet.create(current_action->startCoords.x * cos(-(shoot_angle + current_action->spawnOffsetAngle) / 180 * PI) + current_action->startCoords.y * sin(-(shoot_angle + current_action->spawnOffsetAngle) / 180 * PI) + coords.x, current_action->startCoords.x * cos(-(shoot_angle + current_action->spawnOffsetAngle + 90) / 180 * PI) + current_action->startCoords.y * sin(-(shoot_angle + current_action->spawnOffsetAngle + 90) / 180 * PI) + coords.y, shoot_angle, coords, current_action, &s, target);
		allBullets.push_back(new_bullet);

	}
	void createLaser(gunPlanExemplar *current_action, float shoot_angle, Vector2f coords, player *target) {
		Sprite s;
		s.setTexture(l_example);
		laser actual_laser;
		float laserAngle;
		if (current_action->angleType == 'a') laserAngle = current_action->laserShootAngle;
		else if (current_action->angleType == 'r') laserAngle = LeadAngleToTrigonometric(shoot_angle + current_action->laserShootAngle);
		else if (current_action->angleType == 'p') laserAngle = LeadAngleToTrigonometric(atan2(coords.y - target->playerCoords.y, target->playerCoords.x - coords.x) * 180 / PI) + current_action->laserShootAngle;
		else if (current_action->angleType == 's') {
			if (current_action->startMovingType == 'r') laserAngle = LeadAngleToTrigonometric(atan2(coords.y + current_action->startCoords.y - target->playerCoords.y, target->playerCoords.x - current_action->startCoords.x - coords.x) * 180 / PI) + current_action->laserShootAngle;
			else laserAngle = LeadAngleToTrigonometric(atan2(current_action->startCoords.y - target->playerCoords.y, target->playerCoords.x - current_action->startCoords.x) * 180 / PI) + current_action->laserShootAngle;
		}
		if (current_action->startMovingType == 'a') actual_laser.create(current_action->startCoords.x, current_action->startCoords.y, laserAngle, current_action->laserSize, &s);
		else if (current_action->startMovingType == 'r') actual_laser.create(current_action->startCoords.x + coords.x, current_action->startCoords.y + coords.y, laserAngle, current_action->laserSize, &s);
		else if (current_action->startMovingType == 's') actual_laser.create(current_action->startCoords.x * cos(-(shoot_angle + current_action->spawnOffsetAngle) / 180 * PI) + current_action->startCoords.y * sin(-(shoot_angle + current_action->spawnOffsetAngle) / 180 * PI) + coords.x, current_action->startCoords.x * cos(-(shoot_angle + current_action->spawnOffsetAngle + 90) / 180 * PI) + current_action->startCoords.y * sin(-(shoot_angle + current_action->spawnOffsetAngle + 90) / 180 * PI) + coords.y, laserAngle, current_action->laserSize, &s);
		if (current_beat >= current_action->laserPreparingEndTime) actual_laser.activate();
		else actual_laser.activate_animation(current_beat - current_action->startTime, current_action->laserPreparingEndTime - current_action->startTime);
		allLasers.push_back(actual_laser);
	}
	void clear() {
		allBullets.resize(0);
	}
private:

	std::list<bullet> allBullets;
	std::list<laser> allLasers;
	sf::Texture l_example, example;
};
