#pragma once
const int timerCoof = 600;
const float BPMtoMCSdiv600Converter = (float)pow(60 * 1000000 / timerCoof / 32, 1);
const float PI = 3.141592f;
const std::string gameName = "Touhosu";
float BPM;
float timePerBeat;
float SCREEN_W = 600, SCREEN_H = 600;
float GAMEBOARD_W = 600, GAMEBOARD_H = 600;
float BOARDER = 200;
std::string heroSpriteFile = "Sprites/sprite.png";
std::string bulletsAndHitboxesFile = "Sprites/bullets.png";
std::string turretsFile = "Sprites/enemy.png";
std::string laserFile = "Sprites/laser.png";

float convertForGraphic(float param) {
	return param * SCREEN_H / GAMEBOARD_H;
}
bool isBPMUpdated = false;
float timer = 0;
float current_time;
int current_beat, numberOfBeatThisTurn;
bool newTick;
void timeUpdate(float time) {
	timer += time;
	numberOfBeatThisTurn = 0;
	newTick = false;
	if (timer >= timePerBeat) {
		while (timer > time) {
			timer -= timePerBeat;
			current_beat++;
			if (current_beat % 128 == 0) std::cout << current_beat / 128 << std::endl;
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
void setNewTimePerBeat(float bpm) {
	timePerBeat = BPMtoMCSdiv600Converter / bpm;
	timer = 0;
}