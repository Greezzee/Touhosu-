#pragma once
class button
{
public:
	bool update(RenderWindow *window, float time) {
		bool isMouseOnButton = testForMousePos(window);
		animate(time, isMouseOnButton);
		window->draw(buttonSprite);
		if (isMouseOnButton && Mouse::isButtonPressed(Mouse::Left)) return true;
		return false;
	}
protected:
	Sprite buttonSprite;
	Texture buttonTexture;
	float currentFrame, buttonHeightScale;
	const float Scale = 6;
	void animate(float time, bool isMouseOnButton) {
		if (isMouseOnButton) {
			currentFrame += time * 0.0004f;
			if (currentFrame > 100) 
				currentFrame = 100;
		}
		else {
			currentFrame -= time * 0.0004f;
			if (currentFrame < 0) currentFrame = 0;
		}

		float scale = buttonHeightScale / 10.f * (10.f - 1.f / 100.f * currentFrame);
		buttonSprite.setScale(SCREEN_H / scale / buttonTexture.getSize().y, SCREEN_H / scale / buttonTexture.getSize().y);
	}
	bool testForMousePos(RenderWindow *window) {
		Vector2f realPos;
		realPos.x = buttonSprite.getPosition().x - buttonSprite.getOrigin().x * SCREEN_H / buttonHeightScale / buttonTexture.getSize().y;
		realPos.y = buttonSprite.getPosition().y - buttonSprite.getOrigin().y * SCREEN_H / buttonHeightScale / buttonTexture.getSize().y;
		bool isXdone = false, isYdone = false;
	
		if (Mouse::getPosition(*window).x > realPos.x && Mouse::getPosition(*window).x < realPos.x + buttonSprite.getTexture()->getSize().x * SCREEN_H / buttonHeightScale / buttonTexture.getSize().y) isXdone = true;
		if (Mouse::getPosition(*window).y > realPos.y && Mouse::getPosition(*window).y < realPos.y + buttonSprite.getTexture()->getSize().y * SCREEN_H / buttonHeightScale / buttonTexture.getSize().y) isYdone = true;
		return isXdone && isYdone;
	}

};

class pauseButton : public button
{
public:
	void init(Vector2f startCoords, string type) {
		buttonHeightScale = Scale;
		string fileName = "Sprites/pause-" + type + ".png";
		buttonTexture.loadFromFile(fileName);
		buttonTexture.setSmooth(true);
		buttonSprite.setTexture(buttonTexture);
		buttonSprite.setOrigin(buttonTexture.getSize().x * 0.5, buttonTexture.getSize().y * 0.5);
		buttonSprite.setPosition(startCoords);
		buttonSprite.setScale(SCREEN_H / buttonHeightScale / buttonTexture.getSize().y, SCREEN_H / buttonHeightScale / buttonTexture.getSize().y);
		currentFrame = 0.f;
	}
};


// WORK IN PROGRESS //
class menuButton : public button
{
	void init(Vector2f startCoords, string type) {
		buttonHeightScale = Scale;
		string fileName = "Sprites/pause-" + type + ".png";
		buttonTexture.loadFromFile(fileName);
		buttonTexture.setSmooth(true);
		buttonSprite.setTexture(buttonTexture);
		buttonSprite.setPosition(startCoords);
		buttonSprite.setScale(SCREEN_H / buttonHeightScale / buttonTexture.getSize().y, SCREEN_H / buttonHeightScale / buttonTexture.getSize().y);
		currentFrame = 0.f;
	}
};
///////////////////////

