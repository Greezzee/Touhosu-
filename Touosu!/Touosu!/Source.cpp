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
#include "menuScreens.h"
#include "gameplay.h"
using namespace sf;

int main() {
	gameplay game;
	game.setWindow();
	game.gameStart();
	while (game.getWindowIsOpen()) {
		bool needToClose = game.gameUpdate();
		if (needToClose) break;
	}
	return 0;
}