#pragma once
using namespace sf;

class zone
{
public:
	void init(planner *GlobalMapPlan) {
		zones.resize(0);
		active_zones.resize(0);
		myTexture.loadFromFile("Sprites/zone.png");
		my_sprite.setTexture(myTexture);
		point = 0;

		zones = GlobalMapPlan->getZonePlan();
	}
	void update(vector<vector<sf::Sprite>>& bufferSpriteMap, float time) {
		if (newTick) activate_zone();
		test_for_end();
		bufferSpriteMap[1].push_back(my_sprite);
	}


private:
	vector<zonePlanExemplar> zones;
	std::vector<int> active_zones;
	Sprite my_sprite;
	Texture myTexture;
	unsigned int point;

	void activate_zone() {
		while (point < zones.size() && zones[point].startBeat == current_beat) {
			active_zones.push_back(point);
			point++;
		}
	}
	void test_for_end() {
		for (unsigned int i = 0; i < active_zones.size(); i++)
			if (zones[active_zones[i]].endBeat == current_beat) {
				active_zones.erase(active_zones.begin() + i);
				i--;
			}
	}
	void draw(RenderWindow *window) {
		for (unsigned int i = 0; i < active_zones.size(); i++) {
			if (zones[active_zones[i]].type == "safe") my_sprite.setColor(Color(0, 255, 0, 100));
			else if (zones[active_zones[i]].type == "danger") my_sprite.setColor(Color(255, 0, 0, 100));
			my_sprite.setScale(convertSizeForGraphic(abs(zones[active_zones[i]].UpLeft.x - zones[active_zones[i]].DownRight.x)) / 32, convertSizeForGraphic(abs(zones[active_zones[i]].UpLeft.y - zones[active_zones[i]].DownRight.y)) / 32);
			my_sprite.setPosition(convertPosForGraphic(zones[active_zones[i]].UpLeft));
			window->draw(my_sprite);
		}
	}
};
