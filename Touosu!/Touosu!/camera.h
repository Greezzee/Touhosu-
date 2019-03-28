#pragma once

class camera {
public:
	View cam;
	void init(planner *GlobalMapPlan) {
		cam.reset(sf::FloatRect(0, 0, SCREEN_W, SCREEN_H));
		flashlightTexture.loadFromFile("Sprites/flashlight.png");
		Flashlight.setTexture(flashlightTexture);
		Flashlight.setOrigin(512, 512);
		flashlight_alpha = 0;
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
	int flashlight_alpha;
	std::vector<camPlanExemplar> active_actions;
	camPlanExemplar next_action;
	Sprite Flashlight;
	Texture flashlightTexture;
	bool isActionsEnd;
	void set_new_actions(planner *GlobalMapPlan) {
		if (!isActionsEnd) {
			pair<bool, camPlanExemplar> returned;
			for (int j = numberOfBeatThisTurn - 1; j >= 0; j--) {
				while (true) {
					if (current_beat - j == next_action.startBeat) {
						active_actions.push_back(next_action);

						returned = GlobalMapPlan->getCamPlan();
						if (returned.first) next_action = returned.second;
						else {
							isActionsEnd = true;
							break;
						}
					}
					if (current_beat - j != next_action.startBeat) break;
				}
			}
		}
	}
	bool action(int i, bool new_tick, float player_x, float player_y, RenderWindow *window, float time) {

		if (active_actions[i].type == "flashlight") {

			if (active_actions[i].startBeat + active_actions[i].startAnimationBeat > current_beat) flashlight_alpha = (int)(255 * (((double)current_beat - (double)active_actions[i].startBeat) / (double)active_actions[i].startAnimationBeat));
			else flashlight_alpha = 255;

			if (active_actions[i].endBeat < current_beat) flashlight_alpha = (int)(255 - 255 * (((double)current_beat - (double)active_actions[i].endBeat) / (double)active_actions[i].endAnimationBeat));
			Flashlight.setColor(Color(255, 255, 255, flashlight_alpha));
			Flashlight.setScale(active_actions[i].flashlightSize * 2.5f * SCREEN_H / GAMEBOARD_H, active_actions[i].flashlightSize *  2.5f * SCREEN_H / GAMEBOARD_H);
			Flashlight.setPosition(player_x * SCREEN_H / GAMEBOARD_H, player_y * SCREEN_H / GAMEBOARD_H);
			window->draw(Flashlight);
			if (new_tick && active_actions[i].endBeat + active_actions[i].endAnimationBeat == current_beat)
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
			if (active_actions[i].startBeat + active_actions[i].startBeat > current_beat) blink_alpha = (int)(active_actions[i].blinkAlpha * (((float)current_beat - (float)active_actions[i].startBeat) / (float)active_actions[i].startAnimationBeat));
			else blink_alpha = (int)active_actions[i].blinkAlpha;
			if (active_actions[i].endBeat - active_actions[i].endAnimationBeat <= current_beat) blink_alpha = (int)(active_actions[i].blinkAlpha - active_actions[i].blinkAlpha * (((float)current_beat - (float)active_actions[i].endBeat) / (float)active_actions[i].endAnimationBeat));
			if (blink_alpha < 0) blink_alpha = 0;
			if (blink_alpha > active_actions[i].blinkAlpha) blink_alpha = (int)active_actions[i].blinkAlpha;
			sf::Color myColor = active_actions[i].blinkColor;
			blink.setFillColor(Color(myColor.r, myColor.g, myColor.b, blink_alpha));
			window->draw(blink);
			if (new_tick && active_actions[i].endBeat + active_actions[i].endAnimationBeat == current_beat) return true;
			
			return false;
		}
		return false;
	}
};

