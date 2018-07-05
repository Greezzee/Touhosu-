#pragma once
const int timerCoof = 600;
const double BPMtoMCSdiv600Converter = pow(60 * 1000000 / timerCoof / 32, 1);
const double PI = 3.14159265;
const std::string gameName = "Touhousu";
int BPM;
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
