#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include <string>
#include <fstream>
#include <list>
#include <utility>
#include <stdexcept>
//#include "common.h"

class Director {
private:
    const static std::string PREFIX_SCENE;
    typedef std::pair<std::string, std::list<std::string>> tSceneConfig;
    std::list<tSceneConfig> _scriptConfig;

    void _readScript(std::string &scriptFileName);

public:
    Director(std::string scriptFileName) {
        _readScript(scriptFileName);
    }
};

#endif
