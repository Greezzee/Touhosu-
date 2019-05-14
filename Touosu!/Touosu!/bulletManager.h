#pragma once

std::mutex bulletLock, laserLock;

class bulletManager
{
public:
	void init() {
		allBullets.resize(0);
		allLasers.resize(0);
		bulletsForShoot.resize(0);
		laserForShoot.resize(0);
		bulletTexture.loadFromFile(bulletsAndHitboxesFile);
		lasetTexture.loadFromFile(laserFile);
	}
	void updateAll(RenderWindow *window, float time, player *mainPlayer) {
		vector<bulletCreationInfo> a(0);
		for (list<bullet>::iterator i = allBullets.begin(); i != allBullets.end();) {
			a = i->update(window, time, mainPlayer);
			bulletsForShoot.insert(bulletsForShoot.end(), a.begin(), a.end());
			if (i->destroyed) i = allBullets.erase(i);
			else i++;
		}
		for (list<laser>::iterator i = allLasers.begin(); i != allLasers.end(); i++) {
			i->update(window, mainPlayer);
		}
		createAll(this, mainPlayer);
	}

	

	friend void createBullet(bulletManager *manager, gunPlanExemplar *current_action, float shoot_angle, Vector2f coords, player *target);
	friend void createLaser(bulletManager *manager, gunPlanExemplar *current_action, float shoot_angle, Vector2f coords, player *target);
	friend void createAll(bulletManager *manager, player *mainPlayer);

	void clear() {
		allBullets.resize(0);
	}

	void addBullet(gunPlanExemplar *current_action, float shoot_angle, Vector2f coords) {
		bulletCreationInfo newBullet;
		newBullet.info = *current_action;
		newBullet.angle = shoot_angle;
		newBullet.coords = coords;
		bulletsForShoot.push_back(newBullet);
	}

	void addLaser(gunPlanExemplar *current_action, float shoot_angle, Vector2f coords) {
		bulletCreationInfo newLaser;
		newLaser.info = *current_action;
		newLaser.angle = shoot_angle;
		newLaser.coords = coords;
		laserForShoot.push_back(newLaser);
	}

private:
	vector<bulletCreationInfo> bulletsForShoot, laserForShoot;
	std::list<bullet> allBullets;
	std::list<laser> allLasers;
	sf::Texture lasetTexture, bulletTexture;
};

void createBullet(bulletManager *manager, gunPlanExemplar *current_action, float shoot_angle, Vector2f coords, player *target) {
	Sprite s;
	s.setTexture(manager->bulletTexture);
	s.setTextureRect(IntRect(64, 96, 32, 32));
	s.setOrigin(16, 16);
	bullet new_bullet;
	if (current_action->startMovingType == 'a') new_bullet.create(current_action->startCoords.x, current_action->startCoords.y, shoot_angle, coords, current_action, &s, target);
	else if (current_action->startMovingType == 'r') new_bullet.create(current_action->startCoords.x + coords.x, current_action->startCoords.y + coords.y, shoot_angle, coords, current_action, &s, target);
	else if (current_action->startMovingType == 's') new_bullet.create(current_action->startCoords.x * cos(-(shoot_angle + current_action->spawnOffsetAngle) / 180 * PI) + current_action->startCoords.y * sin(-(shoot_angle + current_action->spawnOffsetAngle) / 180 * PI) + coords.x, current_action->startCoords.x * cos(-(shoot_angle + current_action->spawnOffsetAngle + 90) / 180 * PI) + current_action->startCoords.y * sin(-(shoot_angle + current_action->spawnOffsetAngle + 90) / 180 * PI) + coords.y, shoot_angle, coords, current_action, &s, target);
	bulletLock.lock();
	cout << current_action->bulletInfo.bulletSize[0] << endl;
	manager->allBullets.push_back(new_bullet);
	bulletLock.unlock();
}

void createLaser(bulletManager *manager, gunPlanExemplar *current_action, float shoot_angle, Vector2f coords, player *target) {
	Sprite s;
	s.setTexture(manager->lasetTexture);
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
	laserLock.lock();
	manager->allLasers.push_back(actual_laser);
	laserLock.unlock();
}

void createAll(bulletManager *manager, player *mainPlayer) {
	vector<thread> bulletInitThread, laserInitThread;
	for (unsigned int i = 0; i < manager->bulletsForShoot.size(); i++) {
		bulletInitThread.push_back(std::thread(createBullet, manager, &manager->bulletsForShoot[i].info, manager->bulletsForShoot[i].angle, manager->bulletsForShoot[i].coords, mainPlayer));
	}
	for (unsigned int i = 0; i < manager->laserForShoot.size(); i++) {
		laserInitThread.push_back(std::thread(createLaser, manager, &manager->laserForShoot[i].info, manager->laserForShoot[i].angle, manager->laserForShoot[i].coords, mainPlayer));
	}
	for (auto& t : bulletInitThread) if (t.joinable()) t.join();
	for (auto& t : laserInitThread) if (t.joinable()) t.join();
	manager->bulletsForShoot.clear();
	manager->laserForShoot.clear();
}