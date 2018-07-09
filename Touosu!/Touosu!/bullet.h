#pragma once
using namespace sf;
class bullet {
public:
	void create(double start_x, double start_y, double shoot_angle, sf::Vector2f gunCoords, plan_exemplar *a, Sprite *b, player *target) {

		playerCoords = target->playerCoords;
		myPlan = *a;
		size = a->bulletSize[0] * 0.9;
		nextBulletTypeId = 1;
		prevBulletTypeTime = a->startTime[0];
		coords.x = start_x;
		coords.y = start_y;
		startAnglesSet(shoot_angle, gunCoords);
		startGunAngle = shoot_angle;
		updateBulletSpeedAndAccel(0);

		self_sprite = *b;
		setColor(myPlan.bulletColor[0]);
		self_sprite.setScale(size / 32 * SCREEN_H / GAMEBOARD_H, size / 32 * SCREEN_H / GAMEBOARD_H);
		self_sprite.setPosition(coords.x * SCREEN_H / GAMEBOARD_H, coords.y * SCREEN_H / GAMEBOARD_H);

		actionWithWallID = myPlan.bulletActionWithWalls[0];

		destroyed = false;
	}

	void update(RenderWindow *window, long double time, player *target) {

		playerCoords = target->playerCoords;

		if (coords.x < 0 || coords.x > GAMEBOARD_W || coords.y < 0 || coords.y > GAMEBOARD_H) actionWithWalls();
		for (int i = numberOfBeatThisTurn - 1; i >= 0; i--) tryToTypeUpdate(current_beat - i);

		speed.x += time * acceleration.x;
		speed.y += time * acceleration.y;

		coords.x += speed.x * time;
		coords.y += speed.y * time;

		speedDirectionalAngleRad = atan2(speed.y, speed.x);
		speedDirectionalAngleDegr = RadToDegr(speedDirectionalAngleRad);

		updateAccel();

		self_sprite.setPosition(coords.x * SCREEN_H / GAMEBOARD_H, coords.y * SCREEN_H / GAMEBOARD_H);
		if (pow(coords.x - target->playerCoords.x, 2) + pow(coords.y - target->playerCoords.y, 2) < pow((size + target->size) / 2, 2) && size > 0) target->set_hit();
		window->draw(self_sprite);
	}
	Vector2f coords;
	bool destroyed;
private:
	double speedDirectionalAngleRad, speedDirectionalAngleDegr, accelDirectionalAngleDegr, accelDirectionalAngleRad, size, startGunAngle;
	Vector2f speed, acceleration, playerCoords;
	Sprite self_sprite;
	plan_exemplar myPlan;
	int nextBulletTypeId, prevBulletTypeTime;
	char actionWithWallID;

	void setColor(int colorID) {
		if (colorID == 0) self_sprite.setTextureRect(IntRect((rand() % 16) * 32, 96, 32, 32));
		else self_sprite.setTextureRect(IntRect(colorID * 32, 96, 32, 32));
	}

	void bulletTypeUpdate() {
		size = myPlan.bulletSize[nextBulletTypeId] * 0.9;
		if (size == -1) {
			destroyed = true;
			return;
		}
		self_sprite.setScale(size / 32 * SCREEN_H / GAMEBOARD_H, size / 32 * SCREEN_H / GAMEBOARD_H);
		
		anglesSet();

		updateBulletSpeedAndAccel(nextBulletTypeId);
		setColor(myPlan.bulletColor[nextBulletTypeId]);
		actionWithWallID = myPlan.bulletActionWithWalls[nextBulletTypeId];
		nextBulletTypeId++;
	}
	void updateBulletSpeedAndAccel(int currentTypeNum) {
		if (myPlan.speedChangeType[currentTypeNum] == 'a') {
			speed.x = GAMEBOARD_W / timePerBeat / 32 / 4 * myPlan.lineBulletSpeed[currentTypeNum] * cos(speedDirectionalAngleRad);
			speed.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.lineBulletSpeed[currentTypeNum] * sin(speedDirectionalAngleRad);
		}
		else if (myPlan.speedChangeType[currentTypeNum] == 'r') {
			double fullSpeed = sqrt(speed.x * speed.x + speed.y * speed.y) + myPlan.lineBulletSpeed[currentTypeNum] * GAMEBOARD_W / timePerBeat / 32 / 4;
			speed.x = fullSpeed * cos(speedDirectionalAngleRad);
			speed.y = fullSpeed * sin(speedDirectionalAngleRad);
		}
		acceleration.x = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.lineBulletAccel[currentTypeNum] * cos(accelDirectionalAngleRad);
		acceleration.y = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.lineBulletAccel[currentTypeNum] * sin(accelDirectionalAngleRad);
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

	void startAnglesSet(double gunAngle, sf::Vector2f gunCoords) {
		if (myPlan.speedAngleType[0] == "rel") speedDirectionalAngleDegr = gunAngle + myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "abs") speedDirectionalAngleDegr = myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "plr") speedDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(gunCoords.y - playerCoords.y - myPlan.speedOffsetCoord[0].y, playerCoords.x + myPlan.speedOffsetCoord[0].x - gunCoords.x) * 180 / PI) + myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "splr") speedDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - playerCoords.y - myPlan.speedOffsetCoord[0].y, playerCoords.x + myPlan.speedOffsetCoord[0].x - coords.x) * 180 / PI) + myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "coord") speedDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(gunCoords.y - myPlan.speedOffsetCoord[0].y, myPlan.speedOffsetCoord[0].x - gunCoords.x) * 180 / PI) + myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "scoord") speedDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - myPlan.speedOffsetCoord[0].y, myPlan.speedOffsetCoord[0].x - coords.x) * 180 / PI) + myPlan.bulletSpeedAngle[0];
		else if (myPlan.speedAngleType[0] == "rand") speedDirectionalAngleDegr = rand() % (int)myPlan.bulletSpeedAngle[0];
		speedDirectionalAngleRad = degrToRad(speedDirectionalAngleDegr);

		if (myPlan.accelAngleType[0] == "rel" || myPlan.accelAngleType[0] == "srel") accelDirectionalAngleDegr = speedDirectionalAngleDegr + myPlan.bulletAccelAngle[0];
		else if (myPlan.accelAngleType[0] == "abs") accelDirectionalAngleDegr = myPlan.bulletAccelAngle[0];
		else if (myPlan.accelAngleType[0] == "sabs") accelDirectionalAngleDegr = startGunAngle + myPlan.bulletAccelAngle[0];
		else if (myPlan.accelAngleType[0] == "plr" || myPlan.accelAngleType[0] == "splr") accelDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - playerCoords.y - myPlan.accelOffsetCoord[0].y, playerCoords.x + myPlan.accelOffsetCoord[0].x - coords.x) * 180 / PI) + myPlan.bulletAccelAngle[0];
		else if (myPlan.accelAngleType[0] == "coord" || myPlan.accelAngleType[0] == "scoord") accelDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - myPlan.accelOffsetCoord[0].y, myPlan.accelOffsetCoord[0].x - coords.x) * 180 / PI) + myPlan.bulletAccelAngle[0];
		else if (myPlan.accelAngleType[0] == "rand") accelDirectionalAngleDegr = rand() % (int)myPlan.bulletAccelAngle[0];
		accelDirectionalAngleRad = degrToRad(accelDirectionalAngleDegr);
	}

	void updateAccel() {
		if (myPlan.accelAngleType[nextBulletTypeId - 1] == "srel") accelDirectionalAngleDegr = speedDirectionalAngleDegr + myPlan.bulletAccelAngle[nextBulletTypeId - 1];
		else if (myPlan.accelAngleType[nextBulletTypeId - 1] == "splr") accelDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - playerCoords.y - myPlan.accelOffsetCoord[nextBulletTypeId - 1].y, playerCoords.x + myPlan.accelOffsetCoord[nextBulletTypeId - 1].x - coords.x) * 180 / PI) + myPlan.bulletAccelAngle[nextBulletTypeId - 1];
		else if (myPlan.accelAngleType[nextBulletTypeId - 1] == "scoord") accelDirectionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - myPlan.accelOffsetCoord[nextBulletTypeId - 1].y, myPlan.accelOffsetCoord[nextBulletTypeId - 1].x - coords.x) * 180 / PI) + myPlan.bulletAccelAngle[nextBulletTypeId - 1];
		accelDirectionalAngleRad = degrToRad(accelDirectionalAngleDegr);
		acceleration.x = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.lineBulletAccel[nextBulletTypeId - 1] * cos(accelDirectionalAngleRad);
		acceleration.y = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.lineBulletAccel[nextBulletTypeId - 1] * sin(accelDirectionalAngleRad);
	}

	void actionWithWalls() {
		switch (actionWithWallID)
		{
		case 'd':
			destroyed = true;
			break;

		case 'b':
			if (coords.x < 0 || coords.x > GAMEBOARD_W) {
				speed.x *= -1;
				speedDirectionalAngleRad = atan2(speed.y, speed.x);
				speedDirectionalAngleDegr = LeadAngleToTrigonometric(RadToDegr(speedDirectionalAngleRad));
				speedDirectionalAngleRad = degrToRad(speedDirectionalAngleDegr);
				updateBulletSpeedAndAccel(nextBulletTypeId - 1);
			}
			else if (coords.y < 0 || coords.y > GAMEBOARD_H) {
				speed.y *= -1;
				speedDirectionalAngleRad = atan2(speed.y, speed.x);
				speedDirectionalAngleDegr = LeadAngleToTrigonometric(RadToDegr(speedDirectionalAngleRad));
				speedDirectionalAngleRad = degrToRad(speedDirectionalAngleDegr);
				updateBulletSpeedAndAccel(nextBulletTypeId - 1);
			}
			if (coords.x < 0) coords.x = 0;
			else if (coords.x > GAMEBOARD_W) coords.x = GAMEBOARD_W;
			else if (coords.y < 0) coords.y = 0;
			else if (coords.y > GAMEBOARD_H) coords.y = GAMEBOARD_H;

			break;

		case 'i':
			break;

		case 't':
			if (coords.x < 0) coords.x = GAMEBOARD_W;
			else if (coords.x > GAMEBOARD_W) coords.x = 0;
			else if (coords.y < 0) coords.y = GAMEBOARD_H;
			else if (coords.y > GAMEBOARD_H) coords.y = 0;
			break;
		}
		if (nextBulletTypeId < myPlan.startTime.size() && myPlan.timeType[nextBulletTypeId] == 'w') 
			bulletTypeUpdate();
	}

	double degrToRad(double degr) {
		return -degr / 180 * PI;
	}

	double RadToDegr(double rad) {
		return -rad * 180 / PI;
	}

	void tryToTypeUpdate(int currentBeat) {
		if (nextBulletTypeId < myPlan.startTime.size()) {
			if (myPlan.timeType[nextBulletTypeId] == 'a' && currentBeat == myPlan.startTime[nextBulletTypeId] || myPlan.timeType[nextBulletTypeId] == 'r' && currentBeat == myPlan.startTime[nextBulletTypeId] + prevBulletTypeTime)
				bulletTypeUpdate();
		}
	}
};
