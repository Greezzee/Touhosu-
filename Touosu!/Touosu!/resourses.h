#pragma once
const int timerCoof = 600;
const float BPMtoMCSdiv600Converter = (float)pow(60 * 1000000 / timerCoof / 32, 1);
const float PI = 3.141592f;
const std::string gameName = "Touhosu";
float BPM;
float timePerBeat;
float SCREEN_W = 1600, SCREEN_H = 900;
float GAMEBOARD_W = 600, GAMEBOARD_H = 600;
float BOARDER = 200;
std::string heroSpriteFile = "sprite.png";
std::string bulletsAndHitboxesFile = "bullets.png";
std::string turretsFile = "turret.png";
std::string laserFile = "laser.png";

float convertForGraphic(float param) {
	return param * SCREEN_H / GAMEBOARD_H;
}
bool isBPMUpdated = false;
float timer = 0;
float current_time = -2000000;
int current_beat = 0, numberOfBeatThisTurn;
bool newTick;
void timeUpdate(float time) {
	timer += time;
	numberOfBeatThisTurn = 0;
	newTick = false;
	if (timer >= timePerBeat) {
		while (timer > time) {
			timer -= timePerBeat;
			current_beat++;
			numberOfBeatThisTurn++;
			newTick = true;
		}
	}
}

float LeadAngleToTrigonometric(float angle) {
	if (angle >= 360) angle -= 360;
	else if (angle < 0) angle += 360;
	return angle;
}