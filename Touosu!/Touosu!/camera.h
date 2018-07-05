#pragma once

struct cam_plan {
	string type = "";
	int b_start_time = -1, b_end_time = -1;
	long double cam_center_x = 0, cam_center_y = -1, end_angle = 0, flashlight_size = 0;
	long double zoom = 1;
	bool is_rotate_direction_clockwise = false, is_player_center = false, is_flashlight_active = false;

};

class camera {
public:
	View cam;
	camera(Sprite textur) {
		Flashlight = textur;
		Flashlight.setOrigin(512, 512);
		cam.reset(FloatRect((-SCREEN_W + SCREEN_H - BOARDER * SCREEN_W / SCREEN_H) / 2, -BOARDER / 2, SCREEN_W + BOARDER * SCREEN_W / SCREEN_H, SCREEN_H + BOARDER));
		cam_center_x = 0; cam_center_y = 0; zoom = 1; cam_angle = 0;
		cam_std_coords = cam.getCenter();
		plan.resize(0);
		current_beat = 0;
		timer = 0;
		flashlight_alpha = 0;
		distance_with_player_total = 0;
		distance_with_player_now = 0;
		read_cam_plan();
		player_coords_than_follow_end.x = 0;
		player_coords_than_follow_end.y = 0;
	}
	void update(RenderWindow *window, long double time, long double player_x, long double player_y) {
		timer += time;
		bool new_tick = false;
		if (timer >= timePerBeat)
		{
			timer -= timePerBeat;
			current_beat++;
			new_tick = true;
		}
		if (new_tick) set_new_actions();
		for (int i = 0; i < active_actions.size(); i++) {
			if (action(i, new_tick, player_x, player_y, window, time)) {
				active_actions.erase(active_actions.begin() + i);
				i--;
			}
		}
		window->setView(cam);
	}
private:
	long double cam_center_x, cam_center_y, zoom, buffer_zoom, cam_angle, timer, distance_with_player_total, distance_with_player_now;
	int flashlight_alpha;
	std::vector<cam_plan> plan;
	std::vector<cam_plan> active_actions;
	Sprite Flashlight;
	Vector2f player_coords_than_follow_end;
	Vector2f cam_std_coords;
	int current_step, current_beat;
	void read_cam_plan() {
		current_step = 0;
		ifstream file;
		file.open("plan.txt");
		string command_type;
		plan_exemplar public_bullet;
		do {
			file >> command_type;
			if (command_type == "cam") {
				cam_plan new_plan;
				new_plan.b_start_time = read_time(&file);
				file >> new_plan.type;
				if (new_plan.type == "rotate") {
					new_plan.b_end_time = read_time(&file);
					file >> new_plan.end_angle >> new_plan.is_rotate_direction_clockwise;
				}
				else if (new_plan.type == "zoom") {
					new_plan.b_end_time = read_time(&file);
					file >> new_plan.zoom;
				}
				else if (new_plan.type == "follow") {
					new_plan.b_end_time = read_time(&file);
					new_plan.is_player_center = true;
				}
				else if (new_plan.type == "flashlight") {
					new_plan.b_end_time = read_time(&file);
					file >> new_plan.flashlight_size;
					new_plan.is_player_center = true;
				}
				plan.push_back(new_plan);

			}
		} while (command_type != "end");
		file.close();
	}
	int read_time(ifstream *file) {
		int beats4, beat, beat1_2, beat1_3, beat1_4, beat1_6, beat1_8, beat1_16, beat1_32;
		*file >> beats4 >> beat >> beat1_2 >> beat1_3 >> beat1_4 >> beat1_6 >> beat1_8 >> beat1_16 >> beat1_32;
		return beats4 * 128 + beat * 32 + beat1_2 * 16 + beat1_3 * 11 + beat1_4 * 8 + beat1_6 * 5 + beat1_16 * 2 + beat1_32;
	}

	void set_new_actions() {
		while (true) {
			if (plan.size() > current_step && current_beat == plan[current_step].b_start_time) {
				active_actions.push_back(plan[current_step]);
				current_step++;
			}
			else break;
		}
	}
	bool action(int i, bool new_tick, long double player_x, long double player_y, RenderWindow *window, long double time) {
		if (active_actions[i].type == "rotate") {
			int delta = active_actions[i].b_end_time - active_actions[i].b_start_time;
			long double rotate;
			if (active_actions[i].is_rotate_direction_clockwise) {
				if (abs(cam_angle - active_actions[i].end_angle) <= 180) rotate = -360 + abs(cam_angle - active_actions[i].end_angle);
				else rotate = -abs(cam_angle - active_actions[i].end_angle);
			}
			else {
				if (abs(cam_angle - active_actions[i].end_angle) <= 180) rotate = abs(cam_angle - active_actions[i].end_angle);
				else rotate = 360 - abs(cam_angle - active_actions[i].end_angle);
			}
			if (new_tick) {
				cam.rotate(rotate / delta);
				if (active_actions[i].b_end_time == current_beat) {
					while (true) {
						if (cam_angle >= 360) cam_angle -= 360;
						else if (cam_angle < 0) cam_angle += 360;
						else break;
					}
					cam_angle = active_actions[i].end_angle;
					cam.setRotation(cam_angle);
					return true;
				}
				else return false;
			}
			return false;
		}
		else if (active_actions[i].type == "follow") {
			Vector2f cam_coords = cam.getCenter();
			if (active_actions[i].b_end_time - current_beat <= 32) {
				if (player_coords_than_follow_end.x == 0 && player_coords_than_follow_end.y == 0) {
					player_coords_than_follow_end.x = player_x;
					player_coords_than_follow_end.y = player_y;
				}
				if (new_tick) {
					cam.move((cam_std_coords.x - player_coords_than_follow_end.x * SCREEN_H / GAMEBOARD_H) / 32, (cam_std_coords.y - player_coords_than_follow_end.y * SCREEN_H / GAMEBOARD_H) / 32);
				}
			}
			else {
				if (abs(cam_coords.x - player_x * SCREEN_H / GAMEBOARD_H) <= 15 && abs(cam_coords.y - player_y * SCREEN_H / GAMEBOARD_H) <= 15) cam.setCenter(player_x* SCREEN_H / GAMEBOARD_H, player_y* SCREEN_H / GAMEBOARD_H);
				else cam.move((-cam_coords.x + player_x * SCREEN_H / GAMEBOARD_H) * time * 0.004, (-cam_coords.y + player_y * SCREEN_H / GAMEBOARD_H) * time * 0.004);
			}
			if (active_actions[i].b_end_time == current_beat) {
				distance_with_player_now = 0;
				distance_with_player_total = 0;
				player_coords_than_follow_end.x = 0;
				player_coords_than_follow_end.y = 0;
				cam.reset(FloatRect((-SCREEN_W + SCREEN_H - BOARDER * SCREEN_W / SCREEN_H) / 2, -BOARDER / 2, SCREEN_W + BOARDER * SCREEN_W / SCREEN_H, SCREEN_H + BOARDER));
				cam.setRotation(90 - cam_angle);
				cam.zoom(1 / zoom);
				return true;
			}
			return false;
		}
		else if (active_actions[i].type == "zoom") {
			int delta = active_actions[i].b_end_time - active_actions[i].b_start_time;
			if (buffer_zoom == 0) buffer_zoom = (active_actions[i].zoom - zoom) / delta;
			if (new_tick) {
				zoom += buffer_zoom;
				cam.setSize((SCREEN_W + BOARDER * SCREEN_W / SCREEN_H) / zoom, (SCREEN_H + BOARDER) / zoom);
				if (active_actions[i].b_end_time == current_beat)
				{
					cam.setSize((SCREEN_W + BOARDER * SCREEN_W / SCREEN_H) / active_actions[i].zoom, (SCREEN_H + BOARDER) / active_actions[i].zoom);
					zoom = active_actions[i].zoom;
					buffer_zoom = 0;
					return true;
				}
				else return false;
			}
			return false;
		}

		else if (active_actions[i].type == "flashlight") {
			if (new_tick && flashlight_alpha <= 250 && active_actions[i].b_end_time - current_beat >= 255 / 5) flashlight_alpha += 5;
			Flashlight.setColor(Color(255, 255, 255, flashlight_alpha));
			Flashlight.setScale(active_actions[i].flashlight_size * 2.5 * SCREEN_H / GAMEBOARD_H, active_actions[i].flashlight_size *  2.5 * SCREEN_H / GAMEBOARD_H);
			Flashlight.setPosition(player_x * SCREEN_H / GAMEBOARD_H, player_y * SCREEN_H / GAMEBOARD_H);
			window->draw(Flashlight);
			if (active_actions[i].b_end_time - current_beat < 255 / 15 && new_tick && flashlight_alpha >= 5) flashlight_alpha -= 5;
			if (new_tick && active_actions[i].b_end_time == current_beat)
			{
				flashlight_alpha = 0;
				return true;
			}
			return false;
		}
		return false;
	}
};

