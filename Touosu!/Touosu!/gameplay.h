#pragma once
class gameplay
{
public:
	void setWindow() {
		window.create(sf::VideoMode((unsigned int)SCREEN_W, (unsigned int)SCREEN_H), gameName);
		menu.menuInit();
	}
	void gameStart() {
		GlobalMapPlan.init();
		gameMusic.openFromFile("music.ogg");
		textur.loadFromFile("flashlight.png");
		flash.setTexture(textur);
		heroTexture.loadFromFile(heroSpriteFile);
		bulletsHitboxTexture.loadFromFile(bulletsAndHitboxesFile);
		gunTexture.loadFromFile(turretsFile);
		ZoneTexture.loadFromFile("zone.png");
		zoneSprite.setTexture(ZoneTexture);
		gameboard.setSize(Vector2f(convertForGraphic(GAMEBOARD_W), convertForGraphic(GAMEBOARD_H)));
		gameboard.setFillColor(Color(200, 200, 200));
		setSmoothON();

		restart();
	}
	bool gameUpdate() {
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
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();
			else if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Escape) {
					gameMusic.pause();
					int command = menu.menuPause(&window);
					if (command == 0) {
						clock.restart();
						return true;
					}
					else if (command == 1) {
						gameMusic.play();
						clock.restart();
						window.setView(cam.cam);
						return false;
					}
					else if (command == 2) {
						gameMusic.stop();
						clock.restart();
						window.setView(cam.cam);
						restart();
						return false;
					}
				}
			}
		}
		window.clear(Color(50, 50, 50));
		window.draw(gameboard);
		mainPlayer.update(&window, time);
		if (current_time > 0) {
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
			
			for (list<bullet>::iterator i = allBullets.begin(); i != allBullets.end();) {
				i->update(&window, time, &mainPlayer);
				if (i->destroyed) {
					i = allBullets.erase(i);
					
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
		return false;
	}
	bool getWindowIsOpen() {
		return window.isOpen();
	}

private:
	planner GlobalMapPlan;
	vector<BPMchangeExemplar> bpmChanges;
	Music gameMusic;
	Texture textur;
	Sprite flash, zoneSprite;
	camera cam;
	Clock clock;
	Texture heroTexture, bulletsHitboxTexture, gunTexture, ZoneTexture;
	player mainPlayer;
	std::list<bullet> allBullets;
	std::list<laser> allLasers;
	std::list<gun> allGuns;
	zone zones;
	RectangleShape gameboard;
	int frames, totalBullets;
	float secondTime, FPS, time;
	unsigned int currentBPMid;
	RenderWindow window;
	menuScreens menu;


	void restart() {
		GlobalMapPlan.restart();
		mainPlayer.init(300, 500);
		allBullets.resize(0);
		allLasers.resize(0);
		allGuns.resize(0);
		zones.init(zoneSprite, &GlobalMapPlan);
		bpmChanges.resize(0);
		bpmChanges = GlobalMapPlan.getBPMchangesPlan();
		srand(GlobalMapPlan.getRandomSeed());
		timePerBeat = BPMtoMCSdiv600Converter / BPM;
		cam.init(flash, &GlobalMapPlan);
		for (int i = 0; i < GlobalMapPlan.getNumberOfGuns(); i++) {
			Sprite self_sprite;
			self_sprite.setTexture(gunTexture);
			gun g1;
			g1.set_gun(self_sprite, &GlobalMapPlan, i);
			allGuns.push_back(g1);
		}
		time = 0;
		currentBPMid = 0;
		FPS = 0;
		frames = 0;
		secondTime = 0;
		totalBullets = 0;
		current_beat = 0;
		current_time = -2000000;
		isBPMUpdated = false;
		timer = 0;
		current_time = 0;
		clock.restart();
	}

	void setSmoothON() {
		heroTexture.setSmooth(true);
		bulletsHitboxTexture.setSmooth(true);
		gunTexture.setSmooth(true);
		ZoneTexture.setSmooth(true);
	}
	void setSmoothOFF() {
		heroTexture.setSmooth(false);
		bulletsHitboxTexture.setSmooth(false);
		gunTexture.setSmooth(false);
		ZoneTexture.setSmooth(false);
	}
};



