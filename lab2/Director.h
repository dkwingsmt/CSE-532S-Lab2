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
#include <atomic>

#include "common.h"
#include "Player.h"
#include "Play.h"

class Director {
private:
    const static std::string PREFIX_SCENE;
    std::vector<tFragConfig> _scriptConfig;

    size_t _numThreads;

    std::atomic<Player *> _idler;

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

	~Director() {
		for (auto player : _players) {
			player->join();
		}
	}

    //bool ended() {
    //    return _itNowScene == _scriptConfig.end();
    //}

    // Called by the now-director Player
    // Must call Director::ended() after this function completes!
    void cue(size_t fragId, tCharConfig &charConfig);

    void declareIdle(Player *me);
    bool electDirector();
    
};

#endif
