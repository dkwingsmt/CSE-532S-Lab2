#include "parser.h"
#include <regex>
#include <iostream>

using namespace std;

parser parser::instance;

parser& parser::get_parser() {
	return instance;
}

void parser::set_thread_pool(thread_pool *pool) {
	this->pool = pool;
}

void parser::parse_title(string title) {
	write_to_scene_config("[scene] " + title);
	current_mode = Stateless;
}

bool parser::parse_change(string change_line) {
	const regex change_regex("\\[\\s*(Enter|Exit)\\s+([A-Za-z0-9 ]+)\\.s*\\]");
	smatch match;

	if (regex_search(change_line, match, change_regex) && match.size() > 1) {
		string character = match.str(2);
		if (match.str(1).compare("Enter") == 0) {
			pool->enter(character, act_string.substr(act_index, 1));
			//cout << "pool->enter(" << character << "," << act_string.substr(act_index, 1) << ")" << endl;
			active_characters.push_back(character);
		} else {
			pool->exit(character, act_string.substr(act_index, 1));
			//cout << "pool->exit(" << character << "," << act_string.substr(act_index, 1) << ")" << endl;
			active_characters.erase(remove(active_characters.begin(), active_characters.end(), character), active_characters.end());
		}
		
		current_mode = Change;

		return true;
	} else {
 
		if (current_mode == Change && active_characters.size() > 0) {
			write_to_scene_config(scene_name + act_string.substr(act_index, 1) + "_config.txt");
			write_act_config();
			act_index++;
			line_number = 1;
			//cout << "Change ended" << endl << endl;
		}

		current_mode = Stateless;

		return false;
	}

}

void parser::parse_line(string current_line) {

	if (current_mode == Stateless)
		current_character = "";

	if (current_character.empty()) {
		parse_character_name(current_line);
	} else {
		parse_character_line(current_line);
	}
}

void parser::parse_character_name(string character_name) {

	const regex character_name_regex("^([A-Za-z]+)\\s*\\..*$");
	smatch match;

	if (regex_search(character_name, match, character_name_regex) && match.size() > 1) {
		current_character = match.str(1);
		//cout << "Character Changed to :" << character_name << endl;
		current_mode = Line;
	}
	else
		current_mode = Stateless;

}

void parser::parse_character_line(string character_line) {

	const regex character_line_regex("^([A-Za-z ,.;:'-?!]+)$");
	smatch match;

	if (regex_search(character_line, match, character_line_regex) && match.size() > 1)  {
		pool->submit(current_character, get_line(character_line, line_number++));
		//cout << "pool->submit(" << current_character << ", pline::get_line(" << character_line << ", " << line_number << "))" << endl;
	}
	else
		current_mode = Stateless;
}

void parser::write_act_config() {
	const string act_name = scene_name + act_string.substr(act_index, 1);
	ofstream ofs(base_path + act_name + "_config.txt");

	for_each(active_characters.begin(), active_characters.end(), [&ofs, &act_name](string character_name){
		ofs << character_name << " " << character_name << "_" << act_name << ".txt" << endl;
	});

}

void parser::write_to_scene_config(string line_to_write) {
	*scene_config_file << line_to_write << endl;
}

void parser::parse(string file_name, string scene_name) {

	ifstream ifs(file_name);
	
	this->scene_name = scene_name;
	line_number = 1;
	scene_config_file.reset(new ofstream(base_path + "partial_" + scene_name + "_script.txt"));

	string next_line;

	while (getline(ifs, next_line)) {
		switch (current_mode) {
		case Title:
			parse_title(next_line);
			break;

		case Stateless:
		case Change:
			if(parse_change(next_line))
				break;

		case Line:
			parse_line(next_line);

		}
	}

	scene_config_file.reset(nullptr);

}

void parser::set_base_path(string base_path) {
	this->base_path = base_path;
}

parser::~parser() {
}
