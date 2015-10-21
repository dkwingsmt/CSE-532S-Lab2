#include <string>

struct line {
	std::string line_string;
	int line_number;
	bool change_act;
	bool exit;
};

namespace pline {
	line get_exit_line() {
		line l;
		l.exit = true;
		return std::move(l);
	}
	line get_change_act_line(string act_name) {
		line l;
		l.change_act = true;
		l.line_string = act_name;
		return std::move(l);
	}
}