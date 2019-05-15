#pragma once
using namespace sf;
class trail
{
public:
	void set_trail(Sprite t) {
		lifetime = 1500;
		my_sprite = t;
		my_sprite.setColor(Color(255, 255, 255, 50));
	}
	bool update(vector<vector<sf::Sprite>>& bufferSpriteMap, double time) {
		lifetime -= time;
		my_sprite.setColor(Color(255, 255, 255, (int)lifetime / 15));
		if (lifetime < 0) return true;
		bufferSpriteMap[3].push_back(my_sprite);
		return false;
	}

private:
	long double lifetime;
	Sprite my_sprite;
};

