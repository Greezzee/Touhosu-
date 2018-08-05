#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <vector>
#include <iterator>
#include <iostream>
#include <cmath>
#include <utility>
#include <cstdio>
#include "resourses.h"
#include "planner.h"
#include "player.h"
#include "bullet.h"
#include "laser.h"
#include "gun.h"
#include "zone.h"
#include "camera.h"
using namespace sf;

int main() {
	planner GlobalMapPlan;
	GlobalMapPlan.init();

	vector<BPMchangeExemplar> bpmChanges(0);

	bpmChanges = GlobalMapPlan.getBPMchangesPlan();


	srand(GlobalMapPlan.getRandomSeed());


	Music gameMusic;
    gameMusic.openFromFile("music.ogg");

	timePerBeat = BPMtoMCSdiv600Converter / BPM;
	RenderWindow window(sf::VideoMode((unsigned int)SCREEN_W, (unsigned int)SCREEN_H), gameName);
	Texture textur;
	Sprite flash;
	textur.loadFromFile("flashlight.png");
	flash.setTexture(textur);
	camera cam(flash, &GlobalMapPlan);
	Clock clock;
	Texture heroTexture;
	heroTexture.loadFromFile(heroSpriteFile);
	Texture bulletsHitboxTexture;
	bulletsHitboxTexture.loadFromFile(bulletsAndHitboxesFile);
	Texture gunTexture;
	gunTexture.loadFromFile(turretsFile);
	player mainPlayer(300, 500);
	std::vector<bullet> allBullets(0);
	std::vector<laser> allLasers(0);
	std::vector<gun> allGuns(0);
	Texture ZoneTexture;
	Sprite zoneSprite;
	ZoneTexture.loadFromFile("zone.png");
	zoneSprite.setTexture(ZoneTexture);
	zone zones;
	zones.init(zoneSprite, &GlobalMapPlan);


	for (int i = 0; i < GlobalMapPlan.getNumberOfGuns(); i++) {
		Sprite self_sprite;
		self_sprite.setTexture(gunTexture);
		gun g1;
		g1.set_gun(self_sprite, &GlobalMapPlan, i);
		allGuns.push_back(g1);
	}
	float time = 0;
	RectangleShape gameboard;
	gameboard.setSize(Vector2f(SCREEN_H, SCREEN_H));
	gameboard.setFillColor(Color(200, 200, 200));
	window.draw(gameboard);

	unsigned int currentBPMid = 0;

	while (window.isOpen()) {

		int couter = 1;
		time = (float)clock.getElapsedTime().asMicroseconds();
		current_time += time;
		clock.restart();
		time = time / timerCoof;



		window.clear(Color(50, 50, 50));

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape)) break;

		window.draw(gameboard);

		mainPlayer.update(&window, time);

		if (current_time > 0){
			if (gameMusic.getStatus() == SoundSource::Status::Stopped) {
				gameMusic.play();
				current_time = 0;
			}
			isBPMUpdated = false;
			if (currentBPMid < bpmChanges.size() && current_time >= bpmChanges[currentBPMid].offset) {
				BPM = bpmChanges[currentBPMid].bpm;
				timePerBeat = BPMtoMCSdiv600Converter / BPM;
				currentBPMid++;
				isBPMUpdated = true;
			}


			if (currentBPMid > 0) timeUpdate(time);
			zones.update(&window, time);
			int totalBulletsDestroyed = 0;
			for (unsigned int i = 0; i < allBullets.size(); i++) {
				allBullets[i].update(&window, time, &mainPlayer);
				if (totalBulletsDestroyed < 2 && allBullets[i].destroyed) {
					allBullets.erase(allBullets.begin() + i);
					i--;
					totalBulletsDestroyed++;
				}
			}
			for (unsigned int i = 0; i < allLasers.size(); i++) {
				allLasers[i].update(&window, &mainPlayer);
			}
			allLasers.clear();
			for (unsigned int i = 0; i < allGuns.size(); i++) {
				allGuns[i].update(&window, time, &allBullets, &allLasers, &mainPlayer, &GlobalMapPlan);
			}
			cam.update(&window, time, mainPlayer.playerCoords.x, mainPlayer.playerCoords.y, &GlobalMapPlan);
		}
		else cam.update(&window, 0, mainPlayer.playerCoords.x, mainPlayer.playerCoords.y, &GlobalMapPlan);

		mainPlayer.drawHitbox(&window);

		window.display();
	}
	return 0;
}