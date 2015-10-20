#include "common.h"

// From http://stackoverflow.com/questions/8518743/get-directory-from-file-path-c
string dirnameOf(const string& fname) {
    size_t pos = fname.find_last_of("\\/");
    return (std::string::npos == pos)
        ? ""
        : fname.substr(0, pos+1);
}
