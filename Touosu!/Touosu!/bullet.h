#pragma once
using namespace sf;
class bullet {
public:
	void create(long double start_x, long double start_y, double shoot_angle, plan_exemplar *a, Sprite *b) {
		size = a->bulletSize[0] * 0.9;
		nextBulletTypeId = 1;
		beatSpawnTime = a->startTime[0];
		coords.x = start_x;
		coords.y = start_y;
		directionalAngleDegr = shoot_angle;
		directionalAngleRad = degrToRad(directionalAngleDegr);
		myPlan = *a;

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
		for (int i = numberOfBeatThisTurn - 1; i >= 0; i--) tryToTypeUpdate(current_beat - i);

		coords.x += speed.x * time;
		coords.y += speed.y * time;
		self_sprite.setPosition(coords.x * SCREEN_H / GAMEBOARD_H, coords.y * SCREEN_H / GAMEBOARD_H);
		speed.x += time * acceleration.x;
		speed.y += time * acceleration.y;
		if (pow(coords.x - target->playerCoords.x, 2) + pow(coords.y - target->playerCoords.y, 2) < pow((size + target->size) / 2, 2) && size > 0) target->set_hit();
		if (coords.x < 0 || coords.x > GAMEBOARD_W || coords.y < 0 || coords.y > GAMEBOARD_H) actionWithWalls();
		window->draw(self_sprite);
	}
	Vector2f coords;
	bool destroyed;
private:
	double directionalAngleRad, directionalAngleDegr, size;
	Vector2f speed, acceleration, playerCoords;
	Sprite self_sprite;
	plan_exemplar myPlan;
	int nextBulletTypeId, beatSpawnTime;
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
		if (myPlan.angleType[nextBulletTypeId] == 'r') directionalAngleDegr += myPlan.shootAngle[nextBulletTypeId];
		if (myPlan.angleType[nextBulletTypeId] == 'a') directionalAngleDegr = myPlan.shootAngle[nextBulletTypeId];
		if (myPlan.angleType[nextBulletTypeId] == 'p') directionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - playerCoords.y, playerCoords.x - coords.x) * 180 / PI) + myPlan.shootAngle[nextBulletTypeId];
		directionalAngleRad = degrToRad(directionalAngleDegr);
		updateBulletSpeedAndAccel(nextBulletTypeId);
		setColor(myPlan.bulletColor[nextBulletTypeId]);
		actionWithWallID = myPlan.bulletActionWithWalls[nextBulletTypeId];
		nextBulletTypeId++;
	}
	void updateBulletSpeedAndAccel(int currentTypeNum) {
		if (myPlan.bulletMovmentType[currentTypeNum] == "line") {
			speed.x = GAMEBOARD_W / timePerBeat / 32 / 4 * myPlan.lineBulletSpeed[currentTypeNum] * cos(directionalAngleRad);
			speed.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.lineBulletSpeed[currentTypeNum] * sin(directionalAngleRad);
			acceleration.x = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.lineBulletAccel[currentTypeNum] * cos(directionalAngleRad);
			acceleration.y = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.lineBulletAccel[currentTypeNum] * sin(directionalAngleRad);
		}
		else if (myPlan.bulletMovmentType[currentTypeNum] == "not_line_speed") {
			speed.x = GAMEBOARD_W / timePerBeat / 32 / 4 * myPlan.notLineBulletSpeed[currentTypeNum].x;
			speed.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.notLineBulletSpeed[currentTypeNum].y;
			acceleration.x = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.lineBulletAccel[currentTypeNum] * cos(directionalAngleRad);
			acceleration.y = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.lineBulletAccel[currentTypeNum] * sin(directionalAngleRad);
		}
		else if (myPlan.bulletMovmentType[currentTypeNum] == "not_line_accel") {
			speed.x = GAMEBOARD_W / timePerBeat / 32 / 4 * myPlan.lineBulletSpeed[currentTypeNum] * cos(directionalAngleRad);
			speed.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.lineBulletSpeed[currentTypeNum] * sin(directionalAngleRad);
			acceleration.x = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.notLineBulletAccel[currentTypeNum].x;
			acceleration.y = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.notLineBulletAccel[currentTypeNum].y;
		}
		else {
			speed.x = GAMEBOARD_W / timePerBeat / 32 / 4 * myPlan.notLineBulletSpeed[currentTypeNum].x;
			speed.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.notLineBulletSpeed[currentTypeNum].y;
			acceleration.x = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.notLineBulletAccel[currentTypeNum].x;
			acceleration.y = pow(GAMEBOARD_H / timePerBeat / 32 / 4, 2) * myPlan.notLineBulletAccel[currentTypeNum].y;
		}
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
				directionalAngleRad = atan2(speed.y, speed.x);
				directionalAngleDegr = LeadAngleToTrigonometric(RadToDegr(directionalAngleRad));
				directionalAngleRad = degrToRad(directionalAngleDegr);
				updateBulletSpeedAndAccel(nextBulletTypeId - 1);
			}
			else if (coords.y < 0 || coords.y > GAMEBOARD_H) {
				speed.y *= -1;
				directionalAngleRad = atan2(speed.y, speed.x);
				directionalAngleDegr = LeadAngleToTrigonometric(RadToDegr(directionalAngleRad));
				directionalAngleRad = degrToRad(directionalAngleDegr);
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
		if (nextBulletTypeId < myPlan.startTime.size() - 1 && myPlan.timeType[nextBulletTypeId] == 'w') 
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
			if (myPlan.timeType[nextBulletTypeId - 1] == 'a' && currentBeat == myPlan.startTime[nextBulletTypeId] || myPlan.timeType[nextBulletTypeId - 1] == 'r' && currentBeat == myPlan.startTime[nextBulletTypeId] + beatSpawnTime)
				bulletTypeUpdate();
		}
	}
};
