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
		destroyed = false;
	}

	void update(RenderWindow *window, long double time, player *target) {

		playerCoords = target->playerCoords;
		for (int i = numberOfBeatThisTurn - 1; i >= 0; i--) tryToTypeUpdate(current_beat - i);

		coords.x += speed.x * time;
		coords.y += speed.y * time;
		self_sprite.move(speed.x * time * SCREEN_H / GAMEBOARD_H, speed.y * time * SCREEN_H / GAMEBOARD_H);
		speed.x += time * acceleration.x;
		speed.y += time * acceleration.y;
		if (pow(coords.x - target->playerCoords.x, 2) + pow(coords.y - target->playerCoords.y, 2) < pow((size + target->size) / 2, 2)) target->set_hit();
		if (coords.x < 0 || coords.x > GAMEBOARD_W || coords.y < 0 || coords.y > GAMEBOARD_H) destroyed = true;
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

	void setColor(int colorID) {
		if (colorID == 0) self_sprite.setTextureRect(IntRect((rand() % 16) * 32, 96, 32, 32));
		else self_sprite.setTextureRect(IntRect(colorID * 32, 96, 32, 32));
	}

	void bulletTypeUpdate() {
		size = myPlan.bulletSize[nextBulletTypeId] * 0.9;
		if (myPlan.angleType[nextBulletTypeId] == 'r') directionalAngleDegr += myPlan.shootAngle[nextBulletTypeId];
		if (myPlan.angleType[nextBulletTypeId] == 'a') directionalAngleDegr = myPlan.shootAngle[nextBulletTypeId];
		if (myPlan.angleType[nextBulletTypeId] == 'p') directionalAngleDegr = LeadAngleToTrigonometric(atan2(coords.y - playerCoords.y, playerCoords.x - coords.x) * 180 / PI) + myPlan.shootAngle[nextBulletTypeId];
		directionalAngleRad = degrToRad(directionalAngleDegr);
		updateBulletSpeedAndAccel(nextBulletTypeId);
		setColor(myPlan.bulletColor[nextBulletTypeId]);
		nextBulletTypeId++;
	}
	void updateBulletSpeedAndAccel(int currentTypeNum) {
		if (myPlan.bulletMovmentType[currentTypeNum] == "line") {
			speed.x = GAMEBOARD_W / timePerBeat / 32 / 4 * myPlan.lineBulletSpeed[currentTypeNum] * cos(directionalAngleRad);
			speed.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.lineBulletSpeed[currentTypeNum] * sin(directionalAngleRad);
			acceleration.x = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.lineBulletAccel[currentTypeNum] * cos(directionalAngleRad);
			acceleration.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.lineBulletAccel[currentTypeNum] * sin(directionalAngleRad);
		}
		else if (myPlan.bulletMovmentType[currentTypeNum] == "not_line_speed") {
			speed.x = GAMEBOARD_W / timePerBeat / 32 / 4 * myPlan.notLineBulletSpeed[currentTypeNum].x;
			speed.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.notLineBulletSpeed[currentTypeNum].y;
			acceleration.x = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.lineBulletAccel[currentTypeNum] * cos(directionalAngleRad);
			acceleration.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.lineBulletAccel[currentTypeNum] * sin(directionalAngleRad);
		}
		else if (myPlan.bulletMovmentType[currentTypeNum] == "not_line_accel") {
			speed.x = GAMEBOARD_W / timePerBeat / 32 / 4 * myPlan.lineBulletSpeed[currentTypeNum] * cos(directionalAngleRad);
			speed.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.lineBulletSpeed[currentTypeNum] * sin(directionalAngleRad);
			acceleration.x = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.notLineBulletAccel[currentTypeNum].x;
			acceleration.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.notLineBulletAccel[currentTypeNum].y;
		}
		else {
			speed.x = GAMEBOARD_W / timePerBeat / 32 / 4 * myPlan.notLineBulletSpeed[currentTypeNum].x;
			speed.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.notLineBulletSpeed[currentTypeNum].y;
			acceleration.x = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.notLineBulletAccel[currentTypeNum].x;
			acceleration.y = GAMEBOARD_H / timePerBeat / 32 / 4 * myPlan.notLineBulletAccel[currentTypeNum].y;
		}
	}

	double degrToRad(double degr) {
		return -degr / 180 * PI;
	}

	void tryToTypeUpdate(int currentBeat) {
		if (nextBulletTypeId < myPlan.startTime.size()) {
			if (myPlan.timeType[nextBulletTypeId - 1] == 'a' && currentBeat == myPlan.startTime[nextBulletTypeId] || myPlan.timeType[nextBulletTypeId - 1] == 'r' && currentBeat == myPlan.startTime[nextBulletTypeId] + beatSpawnTime)
				bulletTypeUpdate();
		}
	}
};
