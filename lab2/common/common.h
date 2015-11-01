// Common included headers and utility classes/functions

#ifndef __COMMON_H__
#define __COMMON_H__

#include <functional>
#include <string>
#include <mutex>
#include <algorithm>
#include <cctype>

//#define DEBUG

enum ErrorCode {
	EXCEPTION = 1,
	FILE_NOT_OPEN,
	ARGUMENT_ERROR,
	NO_PLAY_AVAILABLE
};

//The guard object, that performs given procedure upon desctuction. 
class TGuard {
public:
	TGuard(std::function<void(void)> f) : _f(f) {}
	~TGuard() { _f(); }
private:
    std::function<void(void)> _f;
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

static inline std::string trim(std::string s) {
	return ltrim(rtrim(s));
}

std::string dirnameOf(const std::string& fname);

extern std::mutex cout_mutex;

#endif  // header guard
