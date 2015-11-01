#ifndef LAB2_LINE
#define LAB2_LINE

#include <string>

struct line {
	std::string line_string;
	int line_number;
	bool change_act;
	bool exit;
};

inline line get_exit_line() {
	line l;
	l.exit = true;
	l.change_act = false;
	return std::move(l);
}

inline line get_change_act_line(std::string act_name) {
		line l;
		l.change_act = true;
		l.exit = false;
		l.line_string = act_name;
		return std::move(l);
}

inline line get_line(std::string line_string, int line_number) {
		line l;
		l.line_number = line_number;
		l.line_string = line_string;
		l.exit = false; l.change_act = false;
		return std::move(l);
}

#endif