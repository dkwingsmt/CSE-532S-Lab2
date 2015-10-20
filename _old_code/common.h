// Common included headers and utility classes/functions

#ifndef __COMMON_H__
#define __COMMON_H__

#include <cctype>

#include <functional>
#include <thread>
#include <algorithm>

#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <list>
#include <vector>
#include <map>

using namespace std;

enum ErrorCode {
	FILE_NOT_OPEN = 1,
	ARGUMENT_ERROR,
	NO_PLAY_AVAILABLE
};

//The guard object, that performs given procedure upon desctuction. 
class TGuard {
public:
	TGuard(function<void(void)> f) : _f(f) {}
	~TGuard() { _f(); }
private:
	function<void(void)> _f;
};

/*
* trim function from http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
*/
// trim from start
static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends  
static inline std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

string dirnameOf(const string& fname);

#endif  // header guard
