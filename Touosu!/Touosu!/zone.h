#pragma once
using namespace sf;
class zone
{
public:
	void init(Sprite t, double bpm) {
		x_coord1.resize(0);
		y_coord1.resize(0);
		x_coord2.resize(0);
		y_coord2.resize(0);
		start_beat.resize(0);
		end_beat.resize(0);
		type.resize(0);
		active_zones.resize(0);
		my_sprite = t;
		point = 0;
	}
	void update(RenderWindow *window, long double time) {
		if (newTick) activate_zone();
		test_for_end();
		draw(window);
	}

	void read_zones() {
		ifstream file;
		file.open("plan.txt");
		string command_type;
		int sb, eb; long double x1, x2, y1, y2; string t;
		char trash;
		do {
			do {
				file >> command_type;
			} while (command_type != "zone" && command_type != "end");
			if (command_type == "end") break;
			file >> t;
			sb = read_time(&file);
			file >> trash;
			eb = read_time(&file);
			file >> trash;
			file >> x1 >> y1 >> x2 >> y2;
			x_coord1.push_back(x1 * SCREEN_H / GAMEBOARD_H);
			y_coord1.push_back(y1 * SCREEN_H / GAMEBOARD_H);
			x_coord2.push_back(x2 * SCREEN_H / GAMEBOARD_H);
			y_coord2.push_back(y2 * SCREEN_H / GAMEBOARD_H);
			start_beat.push_back(sb);
			end_beat.push_back(eb);
			type.push_back(t);
		} while (command_type != "end");
		file.close();
	}

private:
	std::vector<double> x_coord1, y_coord1, x_coord2, y_coord2;
	std::vector<int> start_beat, end_beat;
	std::vector<string> type;
	std::vector<int> active_zones;
	Sprite my_sprite;
	int point;

	void activate_zone() {
		while (point < start_beat.size() && start_beat[point] == current_beat) {
			active_zones.push_back(point);
			point++;
		}
	}
	void test_for_end() {
		for (int i = 0; i < active_zones.size(); i++)
			if (end_beat[active_zones[i]] == current_beat) {
				active_zones.erase(active_zones.begin() + i);
				i--;
			}
	}
	void draw(RenderWindow *window) {
		for (int i = 0; i < active_zones.size(); i++) {
			if (type[active_zones[i]] == "safe") my_sprite.setColor(Color(0, 255, 0, 100));
			else if (type[active_zones[i]] == "danger") my_sprite.setColor(Color(255, 0, 0, 100));
			my_sprite.setScale(abs(x_coord1[active_zones[i]] - x_coord2[active_zones[i]]) / 32, abs(y_coord1[active_zones[i]] - y_coord2[active_zones[i]]) / 32);
			my_sprite.setPosition(x_coord1[active_zones[i]], y_coord1[active_zones[i]]);
			window->draw(my_sprite);
		}
	}
	int read_time(ifstream *file) {
		int beats4, beat, beat1_2, beat1_3, beat1_4, beat1_6, beat1_8, beat1_16, beat1_32;
		*file >> beats4 >> beat >> beat1_2 >> beat1_3 >> beat1_4 >> beat1_6 >> beat1_8 >> beat1_16 >> beat1_32;
		return beats4 * 128 + beat * 32 + beat1_2 * 16 + beat1_3 * 11 + beat1_4 * 8 + beat1_6 * 5 + beat1_16 * 2 + beat1_32;
	}
};
