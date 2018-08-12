#pragma once

class camera {
public:
	View cam;
	void init(Sprite textur, planner *GlobalMapPlan) {
		Flashlight = textur;
		Flashlight.setOrigin(512, 512);
		cam.reset(FloatRect((-SCREEN_W + SCREEN_H - BOARDER * SCREEN_W / SCREEN_H) / 2, -BOARDER / 2, SCREEN_W + BOARDER * SCREEN_W / SCREEN_H, SCREEN_H + BOARDER));
		cam_center_x = 0; cam_center_y = 0; zoom = 1; cam_angle = 0;
		cam_std_coords = cam.getCenter();
		timer = 0;
		flashlight_alpha = 0;
		distance_with_player_total = 0;
		distance_with_player_now = 0;
		player_coords_than_follow_end.x = 0;
		player_coords_than_follow_end.y = 0;
		isActionsEnd = false;
		pair<bool, camPlanExemplar> returned = GlobalMapPlan->getCamPlan();
		if (returned.first) next_action = returned.second;
		else isActionsEnd = true;
	}
	void update(RenderWindow *window, float time, float player_x, float player_y, planner *GlobalMapPlan) {
		if (newTick) set_new_actions(GlobalMapPlan);
		for (unsigned int i = 0; i < active_actions.size(); i++) {
			if (action(i, newTick, player_x, player_y, window, time)) {
				active_actions.erase(active_actions.begin() + i);
				i--;
			}
		}
		window->setView(cam);
	}
private:
	float cam_center_x, cam_center_y, zoom, buffer_zoom, cam_angle, timer, distance_with_player_total, distance_with_player_now;
	int flashlight_alpha;
	std::vector<camPlanExemplar> active_actions;
	camPlanExemplar next_action;
	Sprite Flashlight;
	Vector2f player_coords_than_follow_end;
	Vector2f cam_std_coords;
	bool isActionsEnd;
	void set_new_actions(planner *GlobalMapPlan) {
		if (!isActionsEnd) {
			pair<bool, camPlanExemplar> returned;
			for (int j = numberOfBeatThisTurn - 1; j >= 0; j--) {
				while (true) {
					if (current_beat - j == next_action.b_start_time) {
						active_actions.push_back(next_action);

						returned = GlobalMapPlan->getCamPlan();
						if (returned.first) next_action = returned.second;
						else {
							isActionsEnd = true;
							break;
						}
					}
					if (current_beat - j != next_action.b_start_time) break;
				}
			}
		}
	}
	bool action(int i, bool new_tick, float player_x, float player_y, RenderWindow *window, float time) {
		if (active_actions[i].type == "rotate") {
			int delta = active_actions[i].b_end_time - active_actions[i].b_start_time;
			float rotate;
			if (active_actions[i].is_rotate_direction_clockwise) {
				if (abs(cam_angle - active_actions[i].end_angle) <= 180) rotate = -abs(cam_angle - active_actions[i].end_angle);
				else rotate = -360 + abs(cam_angle - active_actions[i].end_angle);
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
					cam.setRotation(-cam_angle);
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
				else cam.move((-cam_coords.x + player_x * SCREEN_H / GAMEBOARD_H) * time * 0.004f, (-cam_coords.y + player_y * SCREEN_H / GAMEBOARD_H) * time * 0.004f);
			}
			if (active_actions[i].b_end_time == current_beat) {
				distance_with_player_now = 0;
				distance_with_player_total = 0;
				player_coords_than_follow_end.x = 0;
				player_coords_than_follow_end.y = 0;
				cam.reset(FloatRect((-SCREEN_W + SCREEN_H - BOARDER * SCREEN_W / SCREEN_H) / 2, -BOARDER / 2, SCREEN_W + BOARDER * SCREEN_W / SCREEN_H, SCREEN_H + BOARDER));
				cam.setRotation(-cam_angle);
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

			if (active_actions[i].b_start_time + active_actions[i].b_start_animation > current_beat) flashlight_alpha = (int)(255 * (((double)current_beat - (double)active_actions[i].b_start_time) / (double)active_actions[i].b_start_animation));
			else flashlight_alpha = 255;

			if (active_actions[i].b_end_time < current_beat) flashlight_alpha = (int)(255 - 255 * (((double)current_beat - (double)active_actions[i].b_end_time) / (double)active_actions[i].b_end_animation));
			Flashlight.setColor(Color(255, 255, 255, flashlight_alpha));
			Flashlight.setScale(active_actions[i].flashlight_size * 2.5f * SCREEN_H / GAMEBOARD_H, active_actions[i].flashlight_size *  2.5f * SCREEN_H / GAMEBOARD_H);
			Flashlight.setPosition(player_x * SCREEN_H / GAMEBOARD_H, player_y * SCREEN_H / GAMEBOARD_H);
			window->draw(Flashlight);
			if (new_tick && active_actions[i].b_end_time + active_actions[i].b_end_animation == current_beat)
			{
				flashlight_alpha = 0;
				return true;
			}
			return false;
		}
		else if (active_actions[i].type == "blink") {

			RectangleShape blink(Vector2f(SCREEN_W + 1000, SCREEN_H + 1500));
			blink.setPosition(Vector2f(-1000, -1000));
			int blink_alpha = 0;
			if (active_actions[i].b_start_time + active_actions[i].b_start_time > current_beat) blink_alpha = (int)(active_actions[i].blinkAlpha * (((float)current_beat - (float)active_actions[i].b_start_time) / (float)active_actions[i].b_start_animation));
			else blink_alpha = (int)active_actions[i].blinkAlpha;
			if (active_actions[i].b_end_time - active_actions[i].b_end_animation <= current_beat) blink_alpha = (int)(active_actions[i].blinkAlpha - active_actions[i].blinkAlpha * (((float)current_beat - (float)active_actions[i].b_end_time) / (float)active_actions[i].b_end_animation));
			if (blink_alpha < 0) blink_alpha = 0;
			if (blink_alpha > active_actions[i].blinkAlpha) blink_alpha = (int)active_actions[i].blinkAlpha;
			blink.setFillColor(Color(255, 255, 255, blink_alpha));
			window->draw(blink);
			if (new_tick && active_actions[i].b_end_time + active_actions[i].b_end_animation == current_beat) return true;
			
			return false;
		}
		return false;
	}
};

