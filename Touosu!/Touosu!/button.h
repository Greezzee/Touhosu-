#pragma once
class button
{
public:
	void init(Vector2f origin, Vector2f startCoords, float Scale, string text) {
		buttonHeightScale = Scale;
		buttonTexture.loadFromFile("button.png");
		buttonSprite.setTexture(buttonTexture);
		buttonSprite.setOrigin(buttonTexture.getSize().x * origin.x, buttonTexture.getSize().y * origin.y);
		buttonSprite.setPosition(startCoords);
		buttonDim.setFillColor(Color(219, 112, 147, 150));
		buttonDim.setSize(Vector2f(buttonTexture.getSize().x, buttonTexture.getSize().y));
		buttonDim.setOrigin(buttonDim.getSize().x * origin.x, buttonDim.getSize().y * origin.y);
		buttonDim.setPosition(buttonSprite.getPosition().x, buttonSprite.getPosition().y);
		buttonSprite.setScale(SCREEN_H / buttonHeightScale / buttonTexture.getSize().y, SCREEN_H / buttonHeightScale / buttonTexture.getSize().y);
		buttonDim.setScale(SCREEN_H / buttonHeightScale / buttonTexture.getSize().y, SCREEN_H / buttonHeightScale / buttonTexture.getSize().y);
		currentFrame = 0.f;

		buttonTextFont.loadFromFile("TestFont1.ttf");
		buttonText.setFont(buttonTextFont);
		buttonText.setString(text);
		buttonText.setCharacterSize((unsigned int)(buttonTexture.getSize().y * 0.5f));
		buttonText.setOrigin(Vector2f(buttonText.getLocalBounds().width * origin.x, buttonText.getLocalBounds().height * origin.y));
		buttonText.setPosition(startCoords.x, startCoords.y - buttonText.getLocalBounds().height * origin.y);
	}
	bool update(RenderWindow *window, float time) {
		bool isMouseOnButton = testForMousePos(window);
	
		animate(time, isMouseOnButton);

		window->draw(buttonDim);
		window->draw(buttonText);
		window->draw(buttonSprite);
		if (isMouseOnButton && Mouse::isButtonPressed(Mouse::Left)) return true;
		return false;
	}
private:
	Sprite buttonSprite;
	Texture buttonTexture;
	RectangleShape buttonDim;
	float currentFrame, buttonHeightScale;
	Font buttonTextFont;
	Text buttonText;
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
		buttonDim.setScale(SCREEN_H / scale / buttonTexture.getSize().y, SCREEN_H / scale / buttonTexture.getSize().y);
		buttonText.setScale(SCREEN_H / scale / buttonTexture.getSize().y, SCREEN_H / scale / buttonTexture.getSize().y);
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
