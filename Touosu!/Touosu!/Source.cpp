#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <vector>
#include <iterator>
#include <iostream>
#include <cmath>
#include "resourses.h"
#include "gun_plan.h"
#include "player.h"
#include "bullet.h"
#include "laser.h"
#include "gun.h"
#include "zone.h"
#include "camera.h"
using namespace sf;

int main() {
	ifstream file;
	file.open("plan.txt");
	int NumberOfGuns, randomSeed;
	file >> NumberOfGuns >> randomSeed;
	string trash;

	vector<double> offset, bpm;

	do
	{
		file >> trash;
		if (trash == "new_bpm") {
			offset.push_back(0);
			bpm.push_back(0);
			file >> offset[offset.size() - 1] >> bpm[bpm.size() - 1];
			file >> trash;
		}
	} while (trash != "end");
	srand(randomSeed);
	file.close();


	Music gameMusic;
    gameMusic.openFromFile("music.ogg");

	timePerBeat = BPMtoMCSdiv600Converter / BPM;
	RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H), gameName);
	Texture textur;
	Sprite flash;
	textur.loadFromFile("flashlight.png");
	flash.setTexture(textur);
	camera cam(flash);
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
	std::vector<gun_plan> allGunPlans(0);
	Texture ZoneTexture;
	Sprite zoneSprite;
	ZoneTexture.loadFromFile("zone.png");
	zoneSprite.setTexture(ZoneTexture);
	zone zones;
	zones.init(zoneSprite, BPM);
	zones.read_zones();
	for (int i = 0; i < NumberOfGuns; i++) {
		gun_plan p(i);
		allGunPlans.push_back(p);
	}
	for (int i = 0; i < NumberOfGuns; i++) {
		Sprite self_sprite;
		self_sprite.setTexture(gunTexture);
		gun g1;
		g1.set_gun(self_sprite, allGunPlans[i]);
		allGuns.push_back(g1);
	}
	double time = 0;
	RectangleShape gameboard;
	gameboard.setSize(Vector2f(SCREEN_H, SCREEN_H));
	gameboard.setFillColor(Color(200, 200, 200));
	window.draw(gameboard);

	int currentBPMid = 0;

	while (window.isOpen()) {

		int couter = 1;
		time = clock.getElapsedTime().asMicroseconds();
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
			if (currentBPMid < offset.size() && current_time >= offset[currentBPMid]) {
				BPM = bpm[currentBPMid];
				timePerBeat = BPMtoMCSdiv600Converter / BPM;
				currentBPMid++;
				isBPMUpdated = true;
			}


			if (currentBPMid > 0) timeUpdate(time);
			zones.update(&window, time);
			int totalBulletsDestroyed = 0;
			for (int i = 0; i < allBullets.size(); i++) {
				allBullets[i].update(&window, time, &mainPlayer);
				if (totalBulletsDestroyed < 2 && allBullets[i].destroyed) {
					allBullets.erase(allBullets.begin() + i);
					i--;
					totalBulletsDestroyed++;
				}
			}
			for (int i = 0; i < allLasers.size(); i++) {
				allLasers[i].update(&window, &mainPlayer);
			}
			allLasers.clear();
			for (int i = 0; i < allGuns.size(); i++) {
				allGuns[i].update(&window, time, &allBullets, &allLasers, &mainPlayer);
			}
			cam.update(&window, time, mainPlayer.playerCoords.x, mainPlayer.playerCoords.y);
		}
		else cam.update(&window, 0, mainPlayer.playerCoords.x, mainPlayer.playerCoords.y);
		//camera.setCenter(p1.x_coord, p1.y_coord);
		window.display();
	}
	return 0;
}