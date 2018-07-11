#pragma once
const int timerCoof = 600;
const double BPMtoMCSdiv600Converter = pow(60 * 1000000 / timerCoof / 32, 1);
const double PI = 3.14159265;
const std::string gameName = "Touhousu";
double BPM;
double timePerBeat;
int SCREEN_W = 1280, SCREEN_H = 720;
int GAMEBOARD_W = 600, GAMEBOARD_H = 600;
int BOARDER = 200;
std::string heroSpriteFile = "sprite.png";
std::string bulletsAndHitboxesFile = "bullets.png";
std::string turretsFile = "turret.png";
std::string laserFile = "laser.png";
float convertForGraphic(float param) {
	return param * SCREEN_H / GAMEBOARD_H;
}
bool isBPMUpdated = false;
double timer = 0;
double current_time = -2000000;
int current_beat = 0, numberOfBeatThisTurn;
bool newTick;
void timeUpdate(double time) {
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

double LeadAngleToTrigonometric(double angle) {
	if (angle >= 360) angle -= 360;
	else if (angle < 0) angle += 360;
	return angle;
}