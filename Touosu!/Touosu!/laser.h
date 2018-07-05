#pragma once
using namespace sf;
class laser {
public:
	void create(long double start_x, long double start_y, long double angle, long double sz, Sprite *l) {
		x_coord = start_x;
		y_coord = start_y;
		directional_angle_degr = angle;
		directional_angle_rad = -angle / 360 * 2 * PI;
		size = sz;
		self_sprite = *l;
		self_sprite.setOrigin(-32 / (size / 32 * SCREEN_H / GAMEBOARD_H), 16);
		self_sprite.setScale(size / 32 * SCREEN_H / GAMEBOARD_H, size / 32 * SCREEN_H / GAMEBOARD_H);
		//self_sprite.setOrigin(-24 / size * 32, 16);
		self_sprite.setPosition(x_coord * SCREEN_H / GAMEBOARD_H, y_coord * SCREEN_H / GAMEBOARD_H);
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
	long double x_coord, y_coord;
private:
	long double directional_angle_rad, directional_angle_degr, size;
	Sprite self_sprite;
	bool is_active;

	bool testForHit(player *target) {
		Vector2f NewPlayerCoords;
		NewPlayerCoords.x = x_coord + (target->playerCoords.x - x_coord) * cos(-directional_angle_rad) - (target->playerCoords.y - y_coord) * sin(-directional_angle_rad);
		NewPlayerCoords.y = y_coord + (target->playerCoords.y - y_coord) * cos(-directional_angle_rad) + (target->playerCoords.x - x_coord) * sin(-directional_angle_rad);
		if (NewPlayerCoords.x >= x_coord && NewPlayerCoords.y + target->size >= y_coord - size / 2 && NewPlayerCoords.y - target->size <= y_coord + size / 2) return true;
		else return false;
	}
};



