#pragma once
#include "trail.h"
using namespace sf;
class player {
public:
	Vector2f playerCoords;
	float size, bombCurrentRadius;
	//none for nothing, std for standart bomb
	string currentActiveBomb;
	void init(float start_x, float start_y, string activeSkill) {
		playerCoords.x = start_x;
		playerCoords.y = start_y;
		start_x = convertForGraphic(start_x);
		start_y = convertForGraphic(start_y);

		herotexture.loadFromFile(heroSpriteFile);
		bullets_hitbox.loadFromFile(bulletsAndHitboxesFile);
		herosprite.setTexture(herotexture);
		herosprite.setTextureRect(IntRect(0, 0, 32, 48));
		herosprite.setOrigin(16, 24);
		herosprite.setPosition(start_x, start_y);
		herosprite.setScale(convertForGraphic(1.2f), convertForGraphic(1.2f));

		bombSprite.setTexture(herotexture);
		bombSprite.setOrigin(8, 8);
		bombSprite.setScale(convertForGraphic(1.2f), convertForGraphic(1.2f));

		hero_hitbox_sprite.setTexture(bullets_hitbox);
		hero_hitbox_sprite.setTextureRect(IntRect(843, 140, 38, 38));
		hero_hitbox_sprite.setOrigin(19, 19);
		hero_hitbox_sprite.setPosition(start_x, start_y);
		hero_hitbox_sprite.setScale(convertForGraphic(0.4f), convertForGraphic(0.4f));
		isHitboxVisible = false;

		trail_delta = 0;
		trails.resize(0);

		size = convertForGraphic(2.0f);

		moveDirectionInfo = "";
		prevMoveDirectionInfo = "";
		currentActiveBomb = "none";
		bombCurrentRadius = 0.f;
		currentFrame = 0;
		bombStartTime = 0;
		mySkill = activeSkill;
		bombRotation = 0;
	}
	void update(RenderWindow *window, float time) {
		if (Keyboard::isKeyPressed(Keyboard::Q)) herosprite.setColor(Color(255, 255, 255, 255));;
		testForShift(time);
		trailUpdate(time, window);

		skill(time);
		if (currentActiveBomb == "none" && Keyboard::isKeyPressed(Keyboard::X)) bombLaunch();
		if(currentActiveBomb != "none") bombActive(window, time);
		Move(time);
		updateAnimation(time);
		setSpritePosition();
		window->draw(herosprite);
	}
	void drawHitbox(RenderWindow *window) {
		if (isHitboxVisible) window->draw(hero_hitbox_sprite);
	}
	void set_hit() {
		herosprite.setColor(Color(255, 0, 0, 255));
	}
private:
	float speed;
	float shift_speed;
	float trail_delta;
	float currentFrame;
	/*
	"" for stop
	"r" for right
	"l" for left
	*/
	string moveDirectionInfo, prevMoveDirectionInfo, mySkill;
	float bombStartTime, bombRotation;
	const float MaxBombActiveTime = 3000000.f;
	bool isHitboxVisible;
	Sprite herosprite, hero_hitbox_sprite, bombSprite;
	Texture herotexture, bullets_hitbox;
	std::vector<trail> trails;

	void Move(float time) {
		Vector2f MoveCoef = getMoveCoef();
		playerCoords.x += speed * MoveCoef.x * time;
		playerCoords.y += speed * MoveCoef.y * time;
		updateMoveDirectionInfo(MoveCoef);
	}
	Vector2f getMoveCoef() {
		bool Up = false, Down = false, Left = false, Right = false;
		if (Keyboard::isKeyPressed(Keyboard::Up)) Up = true;
		if (Keyboard::isKeyPressed(Keyboard::Down)) Down = true;
		if (Keyboard::isKeyPressed(Keyboard::Left)) Left = true;
		if (Keyboard::isKeyPressed(Keyboard::Right)) Right = true;
		return testForCollision(getTestMoveCoef(Up, Down, Left, Right));
	}
	Vector2f getTestMoveCoef(bool u, bool d, bool l, bool r) {
		Vector2f MoveCoef(0, 0);
		if ((u || d) && (l || r)) {
			MoveCoef.x = (float)sqrt(2) / 2;
			MoveCoef.y = (float)sqrt(2) / 2;
		}
		else if (u || d) MoveCoef.y = 1;
		else if (l || r) MoveCoef.x = 1;

		if (l) MoveCoef.x *= -1;
		if (u) MoveCoef.y *= -1;
		if (u && d) MoveCoef.y = 0;
		if (l && r) MoveCoef.x = 0;
		return MoveCoef;
	}
	Vector2f testForCollision(Vector2f MoveCoef) {
		if (MoveCoef.x > 0 && playerCoords.x > GAMEBOARD_W) MoveCoef.x = 0;
		if (MoveCoef.x < 0 && playerCoords.x < 0) MoveCoef.x = 0;
		if (MoveCoef.y > 0 && playerCoords.y > GAMEBOARD_H) MoveCoef.y = 0;
		if (MoveCoef.y < 0 && playerCoords.y < 0) MoveCoef.y = 0;
		return MoveCoef;
	}
	void setSpritePosition() {
		herosprite.setPosition(convertForGraphic(playerCoords.x), convertForGraphic(playerCoords.y));
		hero_hitbox_sprite.setPosition(convertForGraphic(playerCoords.x), convertForGraphic(playerCoords.y));
	}
	void testForShift(float time) {
		speed = 0.5f * (float)GAMEBOARD_H / (float)timePerBeat / 32 / 4;
		isHitboxVisible = false;
		if (Keyboard::isKeyPressed(Keyboard::LShift)) {
			speed /= 2;
			isHitboxVisible = true;
		}
	}
	void trailUpdate(float time, RenderWindow *window) {
		for (unsigned int i = 0; i < trails.size(); i++) if (trails[i].update(window, time)) trails.erase(trails.begin() + i);
	}
	void updateMoveDirectionInfo(Vector2f MoveCoef) {
		moveDirectionInfo = "";
		if (MoveCoef.x > 0) moveDirectionInfo += "r";
		else if (MoveCoef.x < 0) moveDirectionInfo += "l";
	}
	void updateAnimation(float time) {
		if (moveDirectionInfo != prevMoveDirectionInfo) currentFrame = 0;
		currentFrame += time * 0.005f;
		prevMoveDirectionInfo = moveDirectionInfo;
		if (moveDirectionInfo == "") {
			if (currentFrame > 8) currentFrame -= 8;
			herosprite.setTextureRect(IntRect(32 * int(currentFrame), 0, 32, 48));
		}
		else if (moveDirectionInfo == "r") {
			if (currentFrame > 8) currentFrame -= 4;
			herosprite.setTextureRect(IntRect(32 * int(currentFrame), 96, 32, 48));
		}
		else if (moveDirectionInfo == "l") {
			if (currentFrame > 8) currentFrame -= 4;
			herosprite.setTextureRect(IntRect(32 * int(currentFrame), 48, 32, 48));
		}
	}

	void skill(float time) {

		if (Keyboard::isKeyPressed(Keyboard::Space)) {
			if (mySkill == "boost") {
				speed *= 2;
				trail_delta += time;
				if (trail_delta >= 50) {
					trail new_trail;
					new_trail.set_trail(herosprite);
					trails.push_back(new_trail);
					trail_delta = 0;
				}
			}
		}
	}
	void bombLaunch() {
		currentActiveBomb = "std";
		bombStartTime = current_time;
	}
	void bombActive(RenderWindow *window, float time) {
		if (current_time > bombStartTime + MaxBombActiveTime && bombStartTime != 0) {
			currentActiveBomb = "none";
			bombStartTime = 0;
			bombCurrentRadius = 0;
			return;
		}

		if (current_time < bombStartTime + 500000.f) bombCurrentRadius = 80.f * (current_time - bombStartTime) / 500000.f;
		else if (current_time > bombStartTime + MaxBombActiveTime - 500000.f) bombCurrentRadius = 80.f * (bombStartTime + MaxBombActiveTime - current_time) / 500000.f;
		else bombCurrentRadius = 80.f;

		bombRotation += time * 0.007f;
		if (bombRotation >= 2 * PI) bombRotation -= 2 * PI;

		for (int i = 0; i < 6; i++) {
			bombSprite.setTextureRect(IntRect(66 + 16 * (i % 3), 146, 16, 16));
			bombSprite.setRotation((PI / 3 * (float)i + bombRotation) / PI * 180);
			bombSprite.setPosition(convertForGraphic(playerCoords.x + bombCurrentRadius * cos(PI / 3 * (float)i + bombRotation)), convertForGraphic(playerCoords.y + bombCurrentRadius * sin(PI / 3 * (float)i + bombRotation)));
			window->draw(bombSprite);
		}
		for (int i = 0; i < 6; i++) {
			bombSprite.setTextureRect(IntRect(66 + 16 * (i % 3), 146, 16, 16));
			bombSprite.setRotation((PI / 3 * (float)i - bombRotation) / PI * 180);
			bombSprite.setPosition(convertForGraphic(playerCoords.x + bombCurrentRadius * cos(PI / 3 * (float)i - bombRotation) / 2), convertForGraphic(playerCoords.y + bombCurrentRadius * sin(PI / 3 * (float)i - bombRotation) / 2));
			window->draw(bombSprite);
		}
	}
};
