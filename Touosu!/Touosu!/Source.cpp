#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <vector>
#include <iterator>
#include <iostream>
#include <cmath>
#include <utility>
#include <cstdio>
#include <list>
#include <thread>
#include <mutex>
#include "soundManager.h"
#include "resourses.h"
#include "planner.h"
#include "player.h"
#include "bullet.h"
#include "laser.h"
#include "bulletManager.h"
#include "gun.h"
#include "zone.h"
#include "camera.h"
#include "button.h"
#include "menuScreens.h"
#include "gameplay.h"
using namespace sf;

int main() {
	gameplay game;
	game.setWindow();
	sf::RenderWindow window(sf::VideoMode((unsigned int)SCREEN_W, (unsigned int)SCREEN_H), gameName);
	window.setActive(false);
	thread drawThread(drawThreadFunc, &game, &window);
	game.gameStart("1");
	while (window.isOpen()) {
		bool needToClose = Update(&game, &window);
		if (needToClose) break;
	}
	if (drawThread.joinable()) drawThread.join();
	return 0;
}