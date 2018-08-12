#pragma once
using namespace sf;
class laser {
public:
	void create(float start_x, float start_y, float angle, float sz, Sprite *l) {
		coords.x = start_x;
		coords.y = start_y;
		directional_angle_degr = angle;
		directional_angle_rad = -angle / 360 * 2 * PI;
		size = sz;
		self_sprite = *l;
		self_sprite.setOrigin(-32 / (convertForGraphic(size) / 32), 16);
		self_sprite.setScale(convertForGraphic(size) / 32, convertForGraphic(size) / 32);
		//self_sprite.setOrigin(-24 / size * 32, 16);
		self_sprite.setPosition(convertForGraphic(coords.x), convertForGraphic(coords.y));
		self_sprite.rotate(-angle);
		self_sprite.setColor(Color(255, 255, 255, 150));
		is_active = false;
	}

	void activate_animation(int current_activation_step, int delta) {
		int currentFrame = (int)(13.0f / (float)delta * (float)current_activation_step);
		self_sprite.setTextureRect(IntRect(0, 32 * (currentFrame + 1), 1642, 32));
		self_sprite.setColor(Color(255, 255, 255, 75));
	}


	void activate() {
		is_active = true;
		self_sprite.setTextureRect(IntRect(0, 0, 1642, 32));
		self_sprite.setColor(Color(255, 255, 255, 150));
	}

	void update(RenderWindow *window, player *target) {
		window->draw(self_sprite);
		if (is_active && testForHit(target)) target->set_hit();
	}
	Vector2f coords;
private:
	float directional_angle_rad, directional_angle_degr, size;
	Sprite self_sprite;
	bool is_active;

	bool testForHit(player *target) {
		Vector2f NewPlayerCoords;
		NewPlayerCoords.x = coords.x + (target->playerCoords.x - coords.x) * cos(-directional_angle_rad) - (target->playerCoords.y - coords.y) * sin(-directional_angle_rad);
		NewPlayerCoords.y = coords.y + (target->playerCoords.y - coords.y) * cos(-directional_angle_rad) + (target->playerCoords.x - coords.x) * sin(-directional_angle_rad);
		if (NewPlayerCoords.x >= coords.x && NewPlayerCoords.y + target->size >= coords.y - size / 2 && NewPlayerCoords.y - target->size <= coords.y + size / 2) return true;
		else return false;
	}
};



