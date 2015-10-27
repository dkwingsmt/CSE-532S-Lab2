#include <iostream>
#include "common.h"

using namespace std;

// From http://stackoverflow.com/questions/8518743/get-directory-from-file-path-c
string dirnameOf(const string& fname) {
    size_t pos = fname.find_last_of("\\/");
    return (string::npos == pos)
        ? ""
        : fname.substr(0, pos+1);
}

mutex cout_mutex;
