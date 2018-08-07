#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <vector>
#include <iterator>
#include <iostream>
#include <cmath>
#include <utility>
#include <cstdio>
#include <list>
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
	std::list<bullet> allBullets(0);
	std::list<laser> allLasers(0);
	std::list<gun> allGuns(0);
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

	float FPS = 0;
	int frames = 0;
	float secondTime = 0;
	int totalBullets = 0;
	while (window.isOpen()) {

		int couter = 1;
		time = (float)clock.getElapsedTime().asMicroseconds();
		secondTime += time;
		frames++;
		if (secondTime >= 1000000) {
			cout << frames << " ";
			cout << totalBullets << endl;
			frames = 0;
			secondTime = 0;
		}
		totalBullets = 0;
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
			int bulletDeleted = 0;
			for (list<bullet>::iterator i = allBullets.begin(); i != allBullets.end();) {
				i->update(&window, time, &mainPlayer);
				if (i->destroyed && bulletDeleted <= 10) {
					i = allBullets.erase(i);
					//bulletDeleted++;
				}
				else i++;
				totalBullets++;
			}

			for (list<laser>::iterator i = allLasers.begin(); i != allLasers.end(); i++) {
				i->update(&window, &mainPlayer);
			}
			allLasers.clear();
			for (list<gun>::iterator i = allGuns.begin(); i != allGuns.end(); i++) {
				i->update(&window, time, &allBullets, &allLasers, &mainPlayer, &GlobalMapPlan);
			}
			cam.update(&window, time, mainPlayer.playerCoords.x, mainPlayer.playerCoords.y, &GlobalMapPlan);
		}
		else cam.update(&window, 0, mainPlayer.playerCoords.x, mainPlayer.playerCoords.y, &GlobalMapPlan);

		mainPlayer.drawHitbox(&window);

		window.display();
	}
	return 0;
}