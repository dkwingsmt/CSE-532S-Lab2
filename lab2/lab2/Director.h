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
    std::shared_ptr<Play> _play;
    std::list<std::shared_ptr<Player>> _players;

    std::atomic<Player *> _idler;
    std::atomic<bool> _hasDirector;

    // Returns biggestPairFrags
    size_t _readScript(std::string &scriptFileName);
    bool _readFragConfig(std::ifstream &, 
                std::list<tCharConfig> &, 
                std::string);
    void _recruit(size_t numPlayers);

public:
    Director(std::string scriptFileName, size_t numberOfPlayers = 0, bool bOverride = false) : 
		_idler(NULL), _hasDirector(false)
    {
        size_t biggestPairFrags = _readScript(scriptFileName);
        _recruit(bOverride ? numberOfPlayers : std::max(biggestPairFrags, numberOfPlayers));
    }

	~Director() {
        // Work threads of players are joined by ~Player
	}

    // Called by the now-director Player
    void cue(size_t fragId, tCharConfig charConfig);

    // Must call Director::ended() after this function completes!
	bool actEnded() { return _play->actEnded(); }

    void declareIdle(Player *me);
    bool electDirector();
	void resign() {
		_hasDirector = false;
	}
    
};

#endif
