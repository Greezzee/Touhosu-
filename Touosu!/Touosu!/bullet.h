#pragma once
using namespace sf;
struct bulletsFromBullets
{
	gunPlanExemplar info;
	float angle;
	Vector2f coords;
};
class bullet {
public:
	void create(float start_x, float start_y, float shoot_angle, sf::Vector2f gunCoords, gunPlanExemplar *a, Sprite *b, player *target) {

		playerCoords = target->playerCoords;
		myPlan = a->bulletInfo;
		colorID = myPlan.bulletColor[0];
		myChildBullets = a->childBullets;
		nextBulletTypeId = 0;
		nextChildBulletID = 0;
		coords.x = start_x;
		coords.y = start_y;
		prevBulletTypeTime = current_beat;
		prevBulletShootTime = current_beat;
		startGunAngle = shoot_angle;

		self_sprite = *b;
		spriteAngle = 0;

		bulletTypeUpdate();
		startAnglesSet(shoot_angle, gunCoords);
		updateBulletSpeedAndAccel(0);

		self_sprite.setPosition(convertForGraphic(coords.x), convertForGraphic(coords.y));
		destroyed = false;

	}

	vector<bulletsFromBullets> update(RenderWindow *window, float time, player *target) {
		if (size == -1) destroyed = true;
		if (destroyed == false) {

			if (isBPMUpdated) updateBulletSpeedAndAccel(nextBulletTypeId - 1);

			playerCoords = target->playerCoords;

			if (coords.x < 0 || coords.x > GAMEBOARD_W || coords.y < 0 || coords.y > GAMEBOARD_H) actionWithWalls();

			bulletsForShoot.resize(0);

			for (int i = numberOfBeatThisTurn - 1; i >= 0; i--) tryToTypeUpdate();

			speed.x += time * acceleration.x * GAMEBOARD_H / timePerBeat / 32 / 4;
			speed.y += time * acceleration.y * GAMEBOARD_H / timePerBeat / 32 / 4;

			coords.x += speed.x * GAMEBOARD_H / timePerBeat / 32 / 4 * time;
			coords.y += speed.y * GAMEBOARD_H / timePerBeat / 32 / 4 * time;

			speedDirectionalAngleRad = atan2(speed.y, speed.x);
			speedDirectionalAngleDegr = RadToDegr(speedDirectionalAngleRad);

			updateAccel();

			self_sprite.setPosition(convertForGraphic(coords.x), convertForGraphic(coords.y));
			animation(time);

			Vector2f NewPlayerCoords;
			if (speed.x != 0 && speed.y != 0) {
				NewPlayerCoords.x = coords.x + (target->playerCoords.x - coords.x) * cos(-speedDirectionalAngleRad) - (target->playerCoords.y - coords.y) * sin(-speedDirectionalAngleRad);
				NewPlayerCoords.y = coords.y + (target->playerCoords.y - coords.y) * cos(-speedDirectionalAngleRad) + (target->playerCoords.x - coords.x) * sin(-speedDirectionalAngleRad);
			}
			else {
				NewPlayerCoords.x = coords.x + (target->playerCoords.x - coords.x) * cos(-degrToRad(myPlan.bulletSpeedAngle[nextBulletTypeId - 1])) - (target->playerCoords.y - coords.y) * sin(-degrToRad(myPlan.bulletSpeedAngle[nextBulletTypeId - 1]));
				NewPlayerCoords.y = coords.y + (target->playerCoords.y - coords.y) * cos(-degrToRad(myPlan.bulletSpeedAngle[nextBulletTypeId - 1])) + (target->playerCoords.x - coords.x) * sin(-degrToRad(myPlan.bulletSpeedAngle[nextBulletTypeId - 1]));
			}
			if (pow((coords.x - NewPlayerCoords.x) * (realEllipseHitboxSize.x + target->size), 2) + pow((coords.y - NewPlayerCoords.y) * (realEllipseHitboxSize.y + target->size), 2) < pow((realEllipseHitboxSize.x + target->size) * (realEllipseHitboxSize.y + target->size), 2) && size > 0) target->set_hit();
			window->draw(self_sprite);
		}
		vector<bulletsFromBullets> out(0);
		for (int i = 0; i < bulletsForShoot.size(); i++) {
			bulletsFromBullets a;
			a.info.bulletInfo = bulletsForShoot[i];
			a.info.startMovingType = bulletsForShoot[i].startMovingType;
			a.info.spawnOffsetAngle = bulletsForShoot[i].spawnOffsetAngle;
			a.info.startCoords = bulletsForShoot[i].startCoords;
			a.angle = speedDirectionalAngleDegr;
			a.coords = coords;
			out.push_back(a);
		}
		return out;
	}
	Vector2f coords;
	bool destroyed;
private:
	float speedDirectionalAngleRad, speedDirectionalAngleDegr, accelDirectionalAngleDegr, accelDirectionalAngleRad, size, startGunAngle, currentFrame, spriteAngle;
	vector<bulletPlanExemplar> myChildBullets, bulletsForShoot;
	Vector2f speed, acceleration, playerCoords, realEllipseHitboxSize;
	Sprite self_sprite;
	bulletPlanExemplar myPlan;
	unsigned int nextBulletTypeId, prevBulletTypeTime, prevBulletShootTime, colorID, nextChildBulletID;
	char actionWithWallID;
	string currentBulletSkin;
	IntRect currentTextureRect;
	bool isRotate, isLookForward;

	void animation(float time) {
		if (isRotate) {
			spriteAngle = LeadAngleToTrigonometric(spriteAngle + timePerBeat * time / 512);
			self_sprite.setRotation(spriteAngle);
		}
		else {
			spriteAngle = 0;
			self_sprite.setRotation(0);
		}

		if (isLookForward) {
			if (speed.x != 0 && speed.y != 0) self_sprite.setRotation(90 - speedDirectionalAngleDegr);
			else self_sprite.setRotation(90 - myPlan.bulletSpeedAngle[nextBulletTypeId - 1]);
		}
		
	}

	void spriteTypeUpdate(int id) {
		currentBulletSkin = myPlan.bulletSkin[id];

		if (colorID == -1) {}
		else if (colorID == 0) colorID = rand() % getIntIDForColor() + 1;
		else colorID = myPlan.bulletColor[id];

		if (colorID != -1) setTextureRect();
		self_sprite.setTextureRect(currentTextureRect);
		setSpriteOrigin();
		setAnimationStyle();
		self_sprite.setScale(convertForGraphic(size) / currentTextureRect.width, convertForGraphic(size) / currentTextureRect.height);
	}
	void setTextureRect() {
		if (currentBulletSkin == "wave") currentTextureRect = IntRect(32 * colorID - 32, 32, 32, 32);
		else if (currentBulletSkin == "circle") currentTextureRect = IntRect(32 * colorID - 32, 64, 32, 32);
		else if (currentBulletSkin == "standart") currentTextureRect = IntRect(32 * colorID - 32, 96, 32, 32);
		else if (currentBulletSkin == "ellipse") currentTextureRect = IntRect(32 * colorID - 32, 128, 32, 31);
		else if (currentBulletSkin == "small_knife") currentTextureRect = IntRect(32 * colorID - 32, 160, 32, 32);
		else if (currentBulletSkin == "crystal") currentTextureRect = IntRect(32 * colorID - 32, 192, 32, 32);
		else if (currentBulletSkin == "rune") currentTextureRect = IntRect(32 * colorID - 32, 224, 32, 32);
		else if (currentBulletSkin == "bullet") currentTextureRect = IntRect(32 * colorID - 32, 256, 32, 32);
		else if (currentBulletSkin == "dark_ellipse") currentTextureRect = IntRect(32 * colorID - 32, 288, 32, 32);
		else if (currentBulletSkin == "small_star") currentTextureRect = IntRect(32 * colorID - 32, 320, 32, 32);
		else if (currentBulletSkin == "alpha_standart") currentTextureRect = IntRect(32 * colorID - 32, 352, 32, 32);
		else if (currentBulletSkin == "small_dark") {
			if (colorID <= 8) currentTextureRect = IntRect(16 * colorID - 16, 384, 16, 16);
			else currentTextureRect = IntRect(16 * (colorID - 8) - 16, 400, 16, 16);
		}
		else if (currentBulletSkin == "small_ellipse") {
			if (colorID <= 8) currentTextureRect = IntRect(32 * colorID - 32 + 128, 384, 32, 32);
			else currentTextureRect = IntRect(32 * (colorID - 8) - 32 + 128, 400, 32, 32);
		}
		else if (currentBulletSkin == "disk") currentTextureRect = IntRect(32 * colorID - 32 + 416, 384, 32, 32);
		else if (currentBulletSkin == "glowing") currentTextureRect = IntRect(64 * colorID - 64, 416, 64, 64);
		else if (currentBulletSkin == "bunny_shit") {
			if (colorID <= 8) currentTextureRect = IntRect(16 * colorID - 16, 480, 16, 16);
			else currentTextureRect = IntRect(16 * (colorID - 8) - 16, 496, 16, 16);
		}
		else if (currentBulletSkin == "big_star") currentTextureRect = IntRect(64 * colorID - 64, 512, 64, 64);
		else if (currentBulletSkin == "big_standart") currentTextureRect = IntRect(64 * colorID - 64, 576, 64, 64);
		else if (currentBulletSkin == "butterfly") currentTextureRect = IntRect(64 * colorID - 64, 640, 64, 64);
		else if (currentBulletSkin == "flower") currentTextureRect = IntRect(64 * colorID - 64, 704, 64, 64);
		else if (currentBulletSkin == "big_knife") currentTextureRect = IntRect(64 * colorID - 64, 768, 64, 64);
		else if (currentBulletSkin == "big_ellipse") currentTextureRect = IntRect(64 * colorID - 64, 832, 64, 64);
		else if (currentBulletSkin == "very_big") currentTextureRect = IntRect(128 * colorID - 128, 896, 128, 128);
		else if (currentBulletSkin == "heart") currentTextureRect = IntRect(512 + 64 * colorID - 64, 0, 64, 64);
		else if (currentBulletSkin == "arrow") currentTextureRect = IntRect(512 + 64 * colorID - 64, 64, 64, 64);
		else if (currentBulletSkin == "small_glowing") currentTextureRect = IntRect(512 + 64 * colorID - 64, 128, 64, 64);
		else if (currentBulletSkin == "wtf") currentTextureRect = IntRect(512 + 32 * colorID - 32, 405, 32, 32);
		else if (currentBulletSkin == "very_big_standart") {
			if (colorID <= 4) currentTextureRect = IntRect(512 + 128 * colorID - 128, 512, 128, 128);
			else currentTextureRect = IntRect(512 + 128 * (colorID - 4) - 128, 640, 128, 128);
		}
	}
	void setSpriteOrigin() {
		if (currentBulletSkin == "wave") self_sprite.setOrigin(16, 22);
		else if (currentBulletSkin == "circle") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "standart") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "small_knife") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "crystal") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "ellipse") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "rune") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "bullet") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "dark_ellipse") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "small_star") self_sprite.setOrigin(16, 17);
		else if (currentBulletSkin == "alpha_standart") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "small_dark") self_sprite.setOrigin(8, 8);
		else if (currentBulletSkin == "small_ellipse") self_sprite.setOrigin(8, 8);
		else if (currentBulletSkin == "disk") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "glowing") self_sprite.setOrigin(32, 32);
		else if (currentBulletSkin == "bunny_shit") self_sprite.setOrigin(8, 8);
		else if (currentBulletSkin == "big_star") self_sprite.setOrigin(32, 34);
		else if (currentBulletSkin == "big_standart") self_sprite.setOrigin(32, 32);
		else if (currentBulletSkin == "butterfly") self_sprite.setOrigin(32, 32);
		else if (currentBulletSkin == "flower") self_sprite.setOrigin(32, 32);
		else if (currentBulletSkin == "big_knife") self_sprite.setOrigin(32, 32);
		else if (currentBulletSkin == "big_ellipse") self_sprite.setOrigin(32, 32);
		else if (currentBulletSkin == "very_big") self_sprite.setOrigin(64, 64);
		else if (currentBulletSkin == "heart") self_sprite.setOrigin(32, 32);
		else if (currentBulletSkin == "arrow") self_sprite.setOrigin(32, 18);
		else if (currentBulletSkin == "small_glowing") self_sprite.setOrigin(32, 32);
		else if (currentBulletSkin == "wtf") self_sprite.setOrigin(16, 16);
		else if (currentBulletSkin == "very_big_standart") self_sprite.setOrigin(64, 64);
	}
	int getIntIDForColor()
	{
		if (currentBulletSkin == "disk") return 3;
		else if (currentBulletSkin == "very_big" || currentBulletSkin == "note") return 4;
		else if (currentBulletSkin == "burning ") return 6;
		else if (currentBulletSkin == "glowing" || currentBulletSkin == "big_star" || currentBulletSkin == "big_standart" || currentBulletSkin == "butterfly" || currentBulletSkin == "flower" || currentBulletSkin == "big_knife" || currentBulletSkin == "big_ellipse" || currentBulletSkin == "heart" || currentBulletSkin == "arrow" || currentBulletSkin == "small_glowing" || currentBulletSkin == "very_big_standart") return 8;
		else return 16;
	}
	void setAnimationStyle() {
		isRotate = false;
		isLookForward = false;

		if (currentBulletSkin == "wave" || currentBulletSkin == "ellipse" || currentBulletSkin == "small_knife" || currentBulletSkin == "crystal" || currentBulletSkin == "rune" || currentBulletSkin == "bullet" || currentBulletSkin == "dark_ellipse" || currentBulletSkin == "small_ellipse" || currentBulletSkin == "butterfly" || currentBulletSkin == "big_knife" || currentBulletSkin == "big_ellipse" || currentBulletSkin == "heart" || currentBulletSkin == "arrow" || currentBulletSkin == "wtf") isLookForward = true;
		else if (currentBulletSkin == "small_star" || currentBulletSkin == "glowing" || currentBulletSkin == "big_star" || currentBulletSkin == "flower" || currentBulletSkin == "very_big") isRotate = true;
	}

	void tryToTypeUpdate() {
		if (nextBulletTypeId < myPlan.startTime.size()) {
			if ((myPlan.timeType[nextBulletTypeId] == 'a' && current_beat >= myPlan.startTime[nextBulletTypeId]) || (myPlan.timeType[nextBulletTypeId] == 'r' && current_beat >= myPlan.startTime[nextBulletTypeId] + (int)prevBulletTypeTime)) {
				bulletTypeUpdate();
			}
		}
		while (true) {
			if (nextChildBulletID >= myChildBullets.size()) break;
			if (myChildBullets[nextChildBulletID].timeType[0] == 'a' && myChildBullets[nextChildBulletID].startTime[0] <= current_beat || myChildBullets[nextChildBulletID].timeType[0] == 'r' && current_beat >= myChildBullets[nextChildBulletID].startTime[0] + (int)prevBulletShootTime) {
				myChildBullets[nextChildBulletID].timeType[0] = 'n';
				myChildBullets[nextChildBulletID].startTime[0] = -1;
				bulletsForShoot.push_back(myChildBullets[nextChildBulletID]);
				nextChildBulletID++;
				prevBulletShootTime = current_beat;
			}
			else break;
		}
	}
	void bulletTypeUpdate() {
		size = myPlan.bulletSize[nextBulletTypeId];
		if (size < 0) {
			destroyed = true;
			return;
		}

		if (myPlan.bulletColor[nextBulletTypeId] != -1) spriteTypeUpdate(nextBulletTypeId);

		setRealHitboxSize();
		anglesSet();

		updateBulletSpeedAndAccel(nextBulletTypeId);
		actionWithWallID = myPlan.bulletActionWithWalls[nextBulletTypeId];
		nextBulletTypeId++;
		prevBulletTypeTime = current_beat;
	}
	void setRealHitboxSize() {
		float realSize = size * 0.9f;
		if (currentBulletSkin == "wave") realEllipseHitboxSize = Vector2f(realSize / 32 * 5, realSize / 32 * 9);
		else if (currentBulletSkin == "circle") realEllipseHitboxSize = Vector2f(realSize / 32 * 14, realSize / 32 * 14);
		else if (currentBulletSkin == "standart") realEllipseHitboxSize = Vector2f(realSize / 32 * 15, realSize / 32 * 15);
		else if (currentBulletSkin == "ellipse") realEllipseHitboxSize = Vector2f(realSize / 32 * 7, realSize / 32 * 13);
		else if (currentBulletSkin == "small_knife") realEllipseHitboxSize = Vector2f(realSize / 32 * 7, realSize / 32 * 16);
		else if (currentBulletSkin == "crystal") realEllipseHitboxSize = Vector2f(realSize / 32 * 7, realSize / 32 * 15);
		else if (currentBulletSkin == "rune") realEllipseHitboxSize = Vector2f(realSize / 32 * 15, realSize / 32 * 15);
		else if (currentBulletSkin == "bullet") realEllipseHitboxSize = Vector2f(realSize / 32 * 8, realSize / 32 * 15);
		else if (currentBulletSkin == "dark_ellipse") realEllipseHitboxSize = Vector2f(realSize / 32 * 6, realSize / 32 * 12);
		else if (currentBulletSkin == "small_star") realEllipseHitboxSize = Vector2f(realSize / 32 * 15, realSize / 32 * 15);
		else if (currentBulletSkin == "alpha_standart") realEllipseHitboxSize = Vector2f(realSize / 32 * 14, realSize / 32 * 14);
		else if (currentBulletSkin == "small_dark") realEllipseHitboxSize = Vector2f(realSize / 16 * 8, realSize / 16 * 8);
		else if (currentBulletSkin == "small_ellipse") realEllipseHitboxSize = Vector2f(realSize / 16 * 6, realSize / 16 * 8);
		else if (currentBulletSkin == "disk") realEllipseHitboxSize = Vector2f(realSize / 32 * 15, realSize / 32 * 15);
		else if (currentBulletSkin == "glowing") realEllipseHitboxSize = Vector2f(realSize / 64 * 17, realSize / 64 * 17);
		else if (currentBulletSkin == "bunny_shit") realEllipseHitboxSize = Vector2f(realSize / 16 * 8, realSize / 16 * 8);
		else if (currentBulletSkin == "big_star") realEllipseHitboxSize = Vector2f(realSize / 64 * 25, realSize / 64 * 25);
		else if (currentBulletSkin == "big_standart") realEllipseHitboxSize = Vector2f(realSize / 64 * 25, realSize / 64 * 25);
		else if (currentBulletSkin == "butterfly") realEllipseHitboxSize = Vector2f(realSize / 64 * 11, realSize / 64 * 11);
		else if (currentBulletSkin == "flower") realEllipseHitboxSize = Vector2f(realSize / 64 * 11, realSize / 64 * 11);
		else if (currentBulletSkin == "big_knife") realEllipseHitboxSize = Vector2f(realSize / 64 * 7, realSize / 64 * 30);
		else if (currentBulletSkin == "big_ellipse") realEllipseHitboxSize = Vector2f(realSize / 64 * 11, realSize / 64 * 23);
		else if (currentBulletSkin == "very_big") realEllipseHitboxSize = Vector2f(realSize / 128 * 42, realSize / 128 * 42);
		else if (currentBulletSkin == "heart") realEllipseHitboxSize = Vector2f(realSize / 64 * 28, realSize / 64 * 28);
		else if (currentBulletSkin == "arrow") realEllipseHitboxSize = Vector2f(realSize / 64 * 10, realSize / 64 * 14);
		else if (currentBulletSkin == "small_glowing") realEllipseHitboxSize = Vector2f(realSize / 64 * 12, realSize / 64 * 12);
		else if (currentBulletSkin == "wtf") realEllipseHitboxSize = Vector2f(realSize / 32 * 9, realSize / 32 * 12);
		else if (currentBulletSkin == "very_big_standart") realEllipseHitboxSize = Vector2f(realSize / 128 * 35, realSize / 128 * 35);
	}

	void updateBulletSpeedAndAccel(int currentTypeNum) {
		if (myPlan.speedChangeType[currentTypeNum] == 'a') {
			speed.x = myPlan.lineBulletSpeed[currentTypeNum] * cos(speedDirectionalAngleRad);
			speed.y = myPlan.lineBulletSpeed[currentTypeNum] * sin(speedDirectionalAngleRad);
		}
		else if (myPlan.speedChangeType[currentTypeNum] == 'r') {
			float fullSpeed = sqrt(speed.x * speed.x + speed.y * speed.y) + myPlan.lineBulletSpeed[currentTypeNum];
			speed.x = fullSpeed * cos(speedDirectionalAngleRad);
			speed.y = fullSpeed * sin(speedDirectionalAngleRad);
		}
		acceleration.x = myPlan.lineBulletAccel[currentTypeNum] * cos(accelDirectionalAngleRad);
		acceleration.y = myPlan.lineBulletAccel[currentTypeNum] * sin(accelDirectionalAngleRad);
	}
	void updateAccel() {
		if (myPlan.accelAngleType[nextBulletTypeId - 1] == "srel") accelDirectionalAngleDegr = speedDirectionalAngleDegr + myPlan.bulletAccelAngle[nextBulletTypeId - 1];
		else if (myPlan.accelAngleType[nextBulletTypeId - 1] == "splr") accelDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - playerCoords.y - myPlan.accelOffsetCoord[nextBulletTypeId - 1].y, playerCoords.x + myPlan.accelOffsetCoord[nextBulletTypeId - 1].x - coords.x) * 180 / PI) + myPlan.bulletAccelAngle[nextBulletTypeId - 1];
		else if (myPlan.accelAngleType[nextBulletTypeId - 1] == "scoord") accelDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - myPlan.accelOffsetCoord[nextBulletTypeId - 1].y, myPlan.accelOffsetCoord[nextBulletTypeId - 1].x - coords.x) * 180 / PI) + myPlan.bulletAccelAngle[nextBulletTypeId - 1];
		accelDirectionalAngleRad = degrToRad(accelDirectionalAngleDegr);
		acceleration.x = myPlan.lineBulletAccel[nextBulletTypeId - 1] * cos(accelDirectionalAngleRad);
		acceleration.y = myPlan.lineBulletAccel[nextBulletTypeId - 1] * sin(accelDirectionalAngleRad);
	}
	void actionWithWalls() {
		switch (actionWithWallID)
		{
		case 'd':
			destroyed = true;
			break;

		case 'b':
			if (coords.x < 0 || coords.x > GAMEBOARD_W) speed.x *= -1;
			else if (coords.y < 0 || coords.y > GAMEBOARD_H) speed.y *= -1;
			speedDirectionalAngleRad = atan2(speed.y, speed.x);
			speedDirectionalAngleDegr = LeadAngleToTrigonometric(RadToDegr(speedDirectionalAngleRad));
			speedDirectionalAngleRad = degrToRad(speedDirectionalAngleDegr);
			updateSpeedAfterBounce();
			if (coords.x < 0.0f) coords.x = 0.0f;
			else if (coords.x > GAMEBOARD_W) coords.x = GAMEBOARD_W;
			else if (coords.y < 0.0f) coords.y = 0.0f;
			else if (coords.y > GAMEBOARD_H) coords.y = GAMEBOARD_H;

			break;

		case 'i':
			break;

		case 't':
			if (coords.x < 0.0f) coords.x = GAMEBOARD_W;
			else if (coords.x > GAMEBOARD_W) coords.x = 0.0f;
			else if (coords.y < 0.0f) coords.y = GAMEBOARD_H;
			else if (coords.y > GAMEBOARD_H) coords.y = 0.0f;
			break;
		case 'c':

			if (coords.x < 0.0f) {
				coords.x = GAMEBOARD_W;
				coords.y = abs(GAMEBOARD_H - coords.y);
			}
			else if (coords.x > GAMEBOARD_W) {
				coords.x = 0.0f;
				coords.y = abs(GAMEBOARD_H - coords.y);
			}
			else if (coords.y < 0.0f) {
				coords.y = GAMEBOARD_H;
				coords.x = abs(GAMEBOARD_W - coords.x);
			}
			else if (coords.y > GAMEBOARD_H) {
				coords.y = 0.0f;
				coords.x = abs(GAMEBOARD_W - coords.x);
			}
			break;
		}
		if (nextBulletTypeId < myPlan.startTime.size() && myPlan.timeType[nextBulletTypeId] == 'w')
			bulletTypeUpdate();
	}
	void updateSpeedAfterBounce() {
		float fullSpeed = sqrt(speed.x * speed.x + speed.y * speed.y);
		speed.x = fullSpeed * cos(speedDirectionalAngleRad);
		speed.y = fullSpeed * sin(speedDirectionalAngleRad);
	}

	void anglesSet() {
		if (myPlan.speedAngleType[nextBulletTypeId] == "rel") speedDirectionalAngleDegr += myPlan.bulletSpeedAngle[nextBulletTypeId];
		else if (myPlan.speedAngleType[nextBulletTypeId] == "abs") speedDirectionalAngleDegr = myPlan.bulletSpeedAngle[nextBulletTypeId];
		else if (myPlan.speedAngleType[nextBulletTypeId] == "splr") speedDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - playerCoords.y - myPlan.speedOffsetCoord[nextBulletTypeId].y, playerCoords.x + myPlan.speedOffsetCoord[nextBulletTypeId].x - coords.x) * 180 / PI) + myPlan.bulletSpeedAngle[nextBulletTypeId];
		else if (myPlan.speedAngleType[nextBulletTypeId] == "scoord") speedDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - myPlan.speedOffsetCoord[nextBulletTypeId].y, myPlan.speedOffsetCoord[nextBulletTypeId].x - coords.x) * 180 / PI) + myPlan.bulletSpeedAngle[nextBulletTypeId];
		else if (myPlan.speedAngleType[nextBulletTypeId] == "rand") speedDirectionalAngleDegr += rand() % (int)myPlan.bulletSpeedAngle[nextBulletTypeId];
		speedDirectionalAngleRad = degrToRad(speedDirectionalAngleDegr);

		if (myPlan.accelAngleType[nextBulletTypeId] == "rel") accelDirectionalAngleDegr += myPlan.bulletAccelAngle[nextBulletTypeId];
		else if (myPlan.accelAngleType[nextBulletTypeId] == "abs") accelDirectionalAngleDegr = myPlan.bulletAccelAngle[nextBulletTypeId];
		else if (myPlan.accelAngleType[nextBulletTypeId] == "sabs") accelDirectionalAngleDegr = startGunAngle + myPlan.bulletAccelAngle[nextBulletTypeId];
		else if (myPlan.accelAngleType[nextBulletTypeId] == "plr") accelDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - playerCoords.y - myPlan.accelOffsetCoord[nextBulletTypeId].y, playerCoords.x + myPlan.accelOffsetCoord[nextBulletTypeId].x - coords.x) * 180 / PI) + myPlan.bulletAccelAngle[nextBulletTypeId];
		else if (myPlan.accelAngleType[nextBulletTypeId] == "coord") accelDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - myPlan.accelOffsetCoord[nextBulletTypeId].y, myPlan.accelOffsetCoord[nextBulletTypeId].x - coords.x) * 180 / PI) + myPlan.bulletAccelAngle[nextBulletTypeId];
		else if (myPlan.accelAngleType[nextBulletTypeId] == "rand") accelDirectionalAngleDegr += rand() % (int)myPlan.bulletAccelAngle[nextBulletTypeId];
		accelDirectionalAngleRad = degrToRad(accelDirectionalAngleDegr);
	}
	void startAnglesSet(float gunAngle, sf::Vector2f gunCoords) {
		if (myPlan.speedAngleType[0] == "rel") speedDirectionalAngleDegr = gunAngle + myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "abs") speedDirectionalAngleDegr = myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "plr") speedDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(gunCoords.y - playerCoords.y - myPlan.speedOffsetCoord[0].y, playerCoords.x + myPlan.speedOffsetCoord[0].x - gunCoords.x) * 180 / PI) + myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "splr") speedDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - playerCoords.y - myPlan.speedOffsetCoord[0].y, playerCoords.x + myPlan.speedOffsetCoord[0].x - coords.x) * 180 / PI) + myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "coord") speedDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(gunCoords.y - myPlan.speedOffsetCoord[0].y, myPlan.speedOffsetCoord[0].x - gunCoords.x) * 180 / PI) + myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "scoord") speedDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - myPlan.speedOffsetCoord[0].y, myPlan.speedOffsetCoord[0].x - coords.x) * 180 / PI) + myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "rand") speedDirectionalAngleDegr = (float)(rand() % (int)myPlan.bulletSpeedAngle[0]);
		speedDirectionalAngleRad = degrToRad(speedDirectionalAngleDegr);

		if (myPlan.accelAngleType[0] == "rel" || myPlan.accelAngleType[0] == "srel") accelDirectionalAngleDegr = speedDirectionalAngleDegr + myPlan.bulletAccelAngle[0];
		else if (myPlan.accelAngleType[0] == "abs") accelDirectionalAngleDegr = myPlan.bulletAccelAngle[0];
		else if (myPlan.accelAngleType[0] == "sabs") accelDirectionalAngleDegr = startGunAngle + myPlan.bulletAccelAngle[0];
		else if (myPlan.accelAngleType[0] == "plr" || myPlan.accelAngleType[0] == "splr") accelDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - playerCoords.y - myPlan.accelOffsetCoord[0].y, playerCoords.x + myPlan.accelOffsetCoord[0].x - coords.x) * 180 / PI) + myPlan.bulletAccelAngle[0];
		else if (myPlan.accelAngleType[0] == "coord" || myPlan.accelAngleType[0] == "scoord") accelDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - myPlan.accelOffsetCoord[0].y, myPlan.accelOffsetCoord[0].x - coords.x) * 180 / PI) + myPlan.bulletAccelAngle[0];
		else if (myPlan.accelAngleType[0] == "rand") accelDirectionalAngleDegr = (float)(rand() % (int)myPlan.bulletAccelAngle[0]);
		accelDirectionalAngleRad = degrToRad(accelDirectionalAngleDegr);
	}
	float degrToRad(float degr) {
		return -degr / 180 * PI;
	}
	float RadToDegr(float rad) {
		return -rad * 180 / PI;
	}

};