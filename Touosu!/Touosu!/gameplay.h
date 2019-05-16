#pragma once

class gameplay
{
private:
	bulletManager manager;
	planner GlobalMapPlan;
	vector<BPMchangeExemplar> bpmChanges;
	Music gameMusic;
	Sprite HUDSprite, background;
	camera cam;
	Clock clock;
	Texture gunTexture, HUDTexture, backgroundTexture;
	player mainPlayer;
	std::list<gun> allGuns;
	zone zones;
	int frames, last_beat = 0;
	float secondTime, FPS, time, backgroundAnimation;
	unsigned int currentBPMid;
	menuScreens menu;
	startPosExemplar start;
	vector<vector<sf::Sprite>> bufferSpriteMap, SpriteMap;

	void restart() {
		soundManager::init();
		GlobalMapPlan.restart();
		mainPlayer.init(192, 400, "boost");
		manager.init();
		allGuns.resize(0);
		zones.init(&GlobalMapPlan);
		bpmChanges.resize(0);
		bpmChanges = GlobalMapPlan.getBPMchangesPlan();
		srand(GlobalMapPlan.getRandomSeed());
		setNewTimePerBeat(BPM);
		cam.init(&GlobalMapPlan);
		start = GlobalMapPlan.getStartPos();
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
		current_beat = start.beatStartTime;
		current_time = -2000000 + start.startTime;
		isBPMUpdated = false;
		timer = 0;
		backgroundAnimation = 0;
		clock.restart();
		bufferSpriteMap.clear();
		bufferSpriteMap.resize(6);
		SpriteMap.clear();
	}	
	void setTextures() {
		HUDSprite.setTexture(HUDTexture);
		background.setTexture(backgroundTexture, true);
	}
	void drawHUD(sf::RenderWindow *window) {
		HUDSprite.setScale(convertSizeForGraphic(1), convertSizeForGraphic(1));
		HUDSprite.setPosition(convertSizeForGraphic(0), convertSizeForGraphic(0));
		HUDSprite.setTextureRect(sf::IntRect(32, 32, 32, 480));
		window->draw(HUDSprite);
		HUDSprite.setPosition(convertSizeForGraphic(32), convertSizeForGraphic(0));
		HUDSprite.setTextureRect(sf::IntRect(32, 512, 384, 16));
		window->draw(HUDSprite);
		HUDSprite.setPosition(convertSizeForGraphic(32), convertSizeForGraphic(464));
		HUDSprite.setTextureRect(sf::IntRect(32, 528, 384, 16));
		window->draw(HUDSprite);
		HUDSprite.setPosition(convertSizeForGraphic(416), convertSizeForGraphic(0));
		HUDSprite.setTextureRect(sf::IntRect(64, 32, 224, 480));
		window->draw(HUDSprite);
	}
	void drawBackground(sf::RenderWindow *window) {

		sf::RectangleShape back;
		back.setFillColor(sf::Color::Black);
		back.setSize({ 384, 480 });
		back.setScale(convertSizeForGraphic(1), convertSizeForGraphic(1));
		back.setPosition(convertPosForGraphic({ 0, 0 }));
		window->draw(back);
		/*
		backgroundAnimation += time * 0.0001;
		if (backgroundAnimation > 1) backgroundAnimation -= 1;
		background.setScale(convertSizeForGraphic(384.f / 256.f), convertSizeForGraphic(384.f / 256.f));

		background.setTextureRect(sf::IntRect(472, 56, 256, 256));
		background.setPosition(convertPosForGraphic({ 0, 0 }));
		window.draw(background);

		background.setTextureRect(sf::IntRect(472, 56, 256, 256));
		background.setPosition(convertPosForGraphic({ 0, 384 }));
		window.draw(background);
		*/
	}
	void drawInformation() {

	}
	friend void drawThreadFunc(gameplay* p, sf::RenderWindow* window);
	friend bool Update(gameplay* p, sf::RenderWindow* window);
public:
	void setWindow() {
		menu.menuInit();
	}
	void gameStart(std::string mapName) {
		GlobalMapPlan.init(mapName);
		gameMusic.openFromFile("Maps/" + mapName + "/music.ogg");
		gunTexture.loadFromFile(turretsFile);
		HUDTexture.loadFromFile("Sprites/HUD.png");
		backgroundTexture.loadFromFile("Sprites/background.png");
		setTextures();
		restart();
	}
	bool logicUpdate() {
		int couter = 1;
		time = (float)clock.getElapsedTime().asMicroseconds();
		secondTime += time;
		frames++;
		current_time += time;
		clock.restart();
		time = time / timerCoof;
		mainPlayer.update(bufferSpriteMap, time);
		if (current_time > start.startTime) {
			
			if (gameMusic.getStatus() == SoundSource::Status::Stopped) {
				Time offsetTime(sf::microseconds((Int64)start.startTime));
				gameMusic.play();
				gameMusic.setPlayingOffset(offsetTime);
				current_time = start.startTime;
			}
			
			isBPMUpdated = false;
			if (currentBPMid < bpmChanges.size() && current_time >= bpmChanges[currentBPMid].offset) {
				BPM = bpmChanges[currentBPMid].bpm;
				setNewTimePerBeat(BPM);
				currentBPMid++;
				isBPMUpdated = true;
			}

			if (last_beat != current_beat) {
				last_beat = current_beat;
			}

			if (currentBPMid > 0) timeUpdate(time);
			zones.update(bufferSpriteMap, time);
			for (list<gun>::iterator i = allGuns.begin(); i != allGuns.end(); i++) {
				i->update(bufferSpriteMap, time, &manager, &mainPlayer, &GlobalMapPlan);
			}
			manager.updateAll(bufferSpriteMap, time, &mainPlayer);
			//cam.update(&window, time, mainPlayer.playerCoords.x, mainPlayer.playerCoords.y, &GlobalMapPlan);
		}
		//else cam.update(&window, 0, mainPlayer.playerCoords.x, mainPlayer.playerCoords.y, &GlobalMapPlan);

		mainPlayer.drawHitbox(bufferSpriteMap);
		return false;
	}
};

mutex drawMutex;
void drawThreadFunc(gameplay* p, sf::RenderWindow *window) {
	while (window->isOpen()) {
		drawMutex.lock();
		auto map = p->SpriteMap;
		p->cam.update(window, p->time, p->mainPlayer.playerCoords.x, p->mainPlayer.playerCoords.y, &p->GlobalMapPlan);
		drawMutex.unlock();
		window->clear(Color(150, 150, 150));
		p->drawBackground(window);
		for (int i = 0; i < map.size(); i++) for (int j = 0; j < map[i].size(); j++) {
			window->draw(map[i][j]);
		}
		p->drawHUD(window);
		window->display();
	}
}

bool Update (gameplay* p, sf::RenderWindow* window) {
	Event event;
	while (window->pollEvent(event)) {
		if (event.type == Event::Closed)
			window->close();
		else if (event.type == Event::KeyPressed) {
			if (event.key.code == Keyboard::Escape) {
				drawMutex.lock();
				window->setActive(true);
				p->gameMusic.pause();
				int command = p->menu.menuPause(window);
				if (command == 0) {
					p->clock.restart();
					window->setActive(false);
					drawMutex.unlock();
					return true;
				}
				else if (command == 1) {
					p->gameMusic.play();
					p->clock.restart();
					window->setView(p->cam.cam);
					window->setActive(false);
					drawMutex.unlock();
					return false;
				}
				else if (command == 2) {
					p->gameMusic.stop();
					p->clock.restart();
					window->setView(p->cam.cam);
					p->restart();
					window->setActive(false);
					drawMutex.unlock();
					return false;
				}
			}
		}
	}
	drawMutex.lock();
	p->SpriteMap = p->bufferSpriteMap;
	drawMutex.unlock();
	p->bufferSpriteMap.clear();
	p->bufferSpriteMap.resize(6);
	bool out = p->logicUpdate();
	return out;
}