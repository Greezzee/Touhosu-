#pragma once
using namespace sf;
class bullet {
public:
	void create(long double start_x, long double start_y, double shoot_angle, plan_exemplar *a, Sprite *b) {
		size = a->shoot_size * 0.9;
		x_coord = start_x;
		y_coord = start_y;
		directional_angle = -shoot_angle / 180 * 3.14159265;
		if (a->shoot_speed != 0) {
			x_speed = GAMEBOARD_W / timePerBeat / 32 / 4 * a->shoot_speed  * cos(directional_angle);
			y_speed = GAMEBOARD_H / timePerBeat / 32 / 4 * a->shoot_speed * sin(directional_angle);
		}
		else {
			x_speed = GAMEBOARD_W / timePerBeat / 32 / 4 * a->shoot_speed_x;
			y_speed = GAMEBOARD_H / timePerBeat / 32 / 4 * a->shoot_speed_y;
		}
		if (a->shoot_acceleration != 0) {
			x_acceleration = GAMEBOARD_H / timePerBeat / 32 / 4 * a->shoot_acceleration * cos(directional_angle);
			y_acceleration = GAMEBOARD_H / timePerBeat / 32 / 4 * a->shoot_acceleration * sin(directional_angle);
		}
		else {
			x_acceleration = GAMEBOARD_H / timePerBeat / 32 / 4 * a->shoot_acceleration_x;
			y_acceleration = GAMEBOARD_H / timePerBeat / 32 / 4 * a->shoot_acceleration_y;
		}
		self_sprite = *b;
		setColor(a->bulletColor);
		self_sprite.setScale(size / 32 * SCREEN_H / GAMEBOARD_H, size / 32 * SCREEN_H / GAMEBOARD_H);
		self_sprite.setPosition(x_coord * SCREEN_H / GAMEBOARD_H, y_coord * SCREEN_H / GAMEBOARD_H);
		destroyed = false;
	}

	void update(RenderWindow *window, long double time, player *target) {
		x_coord += x_speed * time;
		y_coord += y_speed * time;
		self_sprite.move(x_speed * time * SCREEN_H / GAMEBOARD_H, y_speed * time * SCREEN_H / GAMEBOARD_H);
		x_speed += time * x_acceleration;
		y_speed += time * y_acceleration;
		if (pow(x_coord - target->playerCoords.x, 2) + pow(y_coord - target->playerCoords.y, 2) < pow((size + target->size) / 2, 2)) target->set_hit();
		if (x_coord < 0 || x_coord > GAMEBOARD_W || y_coord < 0 || y_coord > GAMEBOARD_H) destroyed = true;
		window->draw(self_sprite);
	}
	long double x_coord, y_coord;
	bool destroyed;
private:
	long double directional_angle, x_speed, y_speed, size, x_acceleration, y_acceleration;
	Sprite self_sprite;

	void setColor(int colorID)
	{
		if (colorID == 0) self_sprite.setTextureRect(IntRect((rand() % 16) * 32, 96, 32, 32));
		
		else self_sprite.setTextureRect(IntRect(colorID * 32, 96, 32, 32));
	}
};

