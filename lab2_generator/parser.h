#ifndef __PARSER_H__
#define __PARSER_H__

#include "thread_pool.h"
#include <memory>
#include <fstream>

enum mode {
	Title, Change, Line, Stateless
};

class parser
{
	mode					   current_mode;
	thread_pool				   *pool;
	string				       scene_name;
	int						   line_number;
	string					   current_character;
	vector<string>			   active_characters;
	int				    	   act_index;
	unique_ptr<ofstream>	   scene_config_file;
	string					   base_path;

	const string act_string;

	static parser instance;

	parser() : current_mode(Title), act_string("abcdefghijklmnopqrstuvwxyz"), act_index(0), line_number(1) {}

	void parse_title(string line);

	bool parse_change(string line);

	void parse_line(string line);

	void parse_character_name(string line);

	void parse_character_line(string line);

	void write_to_scene_config(string line);

	void write_act_config();

public:

	parser(const parser&) = delete;

	parser& operator=(const parser&) = delete;

	static parser& get_parser();

	void set_thread_pool(thread_pool *pool);

	void set_base_path(string base_path);

	void parse(string file_name, string scene_name);

	~parser();
};

#endif
