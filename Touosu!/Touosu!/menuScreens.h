#pragma once
#pragma warning(disable : 4996)
class menuScreens
{
public:
	void menuInit() {

	}
	//return 0 if exit
	//return 1 if game continue
	//return 2 if restart
	int menuPause(RenderWindow *window) {
		bool isAbleToClose = false;
		Image playimage = window->capture();
		Texture buffer;
		Vector2u size = playimage.getSize();
		buffer.create(size.x, size.y);
		buffer.update(playimage);
		Sprite playground;
		playground.setTexture(buffer);
		playground.setPosition((-SCREEN_W + SCREEN_H - BOARDER * SCREEN_W / SCREEN_H) / 2, -BOARDER / 2);
		playground.setScale(Vector2f((SCREEN_W + BOARDER * SCREEN_W / SCREEN_H) / SCREEN_W, (SCREEN_H + BOARDER) / SCREEN_H));
		while (true) {
			Event event;
			while (window->pollEvent(event)) {
				if (event.type == Event::Closed)
					window->close();
				else if (event.type == Event::KeyPressed) {
					if (event.key.code == Keyboard::Escape && isAbleToClose) return 1;
				}
				else if (event.type == Event::KeyReleased) {
					if (event.key.code == Keyboard::Escape) isAbleToClose = true;
				}
			}
			window->clear();
			window->draw(playground);
			window->display();
		}
	}
	void gameOverMenu() {

	}
private:

};

