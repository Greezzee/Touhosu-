#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iterator>
#include <iostream>
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



	Music music;
	music.openFromFile("audio.ogg");
	BPM = 120;
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
	player mainPlayer(50, 250);
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
	for (int i = 0; i < 1; i++) {
		gun_plan p(i);
		allGunPlans.push_back(p);
	}
	for (int i = 0; i < 1; i++) {
		Sprite self_sprite;
		self_sprite.setTexture(gunTexture);
		gun g1;
		g1.set_gun(self_sprite, allGunPlans[i]);
		allGuns.push_back(g1);
	}
	double offset = 0, pause = 0, time = 0;
	RectangleShape gameboard;
	gameboard.setSize(Vector2f(SCREEN_H, SCREEN_H));
	gameboard.setFillColor(Color(200, 200, 200));
	window.draw(gameboard);
	while (window.isOpen()) {

		int couter = 1;
		time = clock.getElapsedTime().asMicroseconds();
		pause += time;
		offset += time;
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

		if (pause > 2000000){
			//if (offset >= 516 + 2000000 && music.getStatus() == SoundSource::Status::Stopped) music.play();
			timeUpdate(time);
			zones.update(&window, time);
			for (int i = 0; i < allBullets.size(); i++) {
				allBullets[i].update(&window, time, &mainPlayer);
				if (allBullets[i].destroyed) {
					allBullets.erase(allBullets.begin() + i);
					i--;
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