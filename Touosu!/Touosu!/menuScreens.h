#pragma once
#pragma warning(disable : 4996)
class menuScreens
{
public:
	void menuInit() {
		menuCam.reset(FloatRect(0, 0, SCREEN_W, SCREEN_H));
	}
	//return 0 if exit
	//return 1 if game continue
	//return 2 if restart
	int menuPause(RenderWindow *window) {

		menuClock.restart();

		window->setView(menuCam);
		bool isAbleToClose = false;
		Image playimage = window->capture();
		Texture buffer;
		Vector2u size = playimage.getSize();
		buffer.create(size.x, size.y);
		buffer.update(playimage);
		Sprite playground;
		RectangleShape backgroundDim;
		backgroundDim.setFillColor(Color(0, 0, 0, 150));
		backgroundDim.setSize(Vector2f(SCREEN_W, SCREEN_H));
		playground.setTexture(buffer);

		button continueButton, restartButton, exitButton;
		continueButton.init(Vector2f(0.5f, 0.5f), Vector2f(SCREEN_W / 2, SCREEN_H / 4), 6, "CONTINUE");
		restartButton.init(Vector2f(0.5f, 0.5f), Vector2f(SCREEN_W / 2, SCREEN_H / 4 * 2), 6, "RESTART");
		exitButton.init(Vector2f(0.5f, 0.5f), Vector2f(SCREEN_W / 2, SCREEN_H / 4 * 3), 6, "EXIT");

		window->clear();
		while (true) {
			Event event;
			while (window->pollEvent(event)) {
				if (event.type == Event::Closed)
					window->close();
				else if (event.type == Event::KeyPressed) {
					if (event.key.code == Keyboard::Escape && isAbleToClose) {
						window->clear();
						return 1;
					}
				}
				else if (event.type == Event::KeyReleased) {
					if (event.key.code == Keyboard::Escape) isAbleToClose = true;
				}
			}
			window->clear();

			window->draw(playground);
			window->draw(backgroundDim);

			float time = menuClock.getElapsedTime().asMicroseconds();
			menuClock.restart();

			if (continueButton.update(window, time)) return 1;
			if (restartButton.update(window, time)) return 2;
			if (exitButton.update(window, time)) return 0;

			window->display();
		}
	}
	void gameOverMenu() {

	}
private:
	View menuCam;
	Clock menuClock;
};

