#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include <string>
#include <fstream>
#include <list>
#include <utility>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <stdexcept>

#include "common.h"
#include "Player.h"
#include "Play.h"

class Director {
private:
    const static std::string PREFIX_SCENE;
    std::vector<tFragConfig> _scriptConfig;

    size_t _numThreads;

    Player *_idler;
    std::mutex _hasIdleMutex;
    std::condition_variable _hasIdleCv;

    std::shared_ptr<Play> _play;
    std::list<std::shared_ptr<Player>> _players;

    // Returns biggestPairFrags
    size_t _readScript(std::string &scriptFileName);
    bool _readFragConfig(std::ifstream &, std::list<tCharConfig> &, std::string);
    void _recruit(size_t numPlayers);

public:

    Director(std::string scriptFileName) : 
        _numThreads(0),
        _idler(NULL)
    {
        size_t biggestPairFrags = _readScript(scriptFileName);
        std::cout << "Total of " << biggestPairFrags << " threads." << std::endl;
        // TODO: customized numPlayers
        _recruit(biggestPairFrags);
    }

    //bool ended() {
    //    return _itNowScene == _scriptConfig.end();
    //}

    void start();

    // Called by the now-director Player
    // Must call Director::ended() after this function completes!
    bool cue();

    void declareIdle(Player *me);
};

#endif
