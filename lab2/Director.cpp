#include <sstream>
#include "Director.h"
#include "common.h"

using namespace std;

const string Director::PREFIX_SCENE = "[scene]";

size_t Director::_readScript(string &scriptFileName) {
    ifstream scriptFile(scriptFileName);
    if (!scriptFile) {
        throw invalid_argument(scriptFileName);
    }

    string line;
    string scene_title;
    string dir = dirnameOf(scriptFileName);
    size_t biggestPairFrags = 0;
    size_t sizeLastFrag = 0;
    while (getline(scriptFile, line)) {
        if (line[0] == '[') {
            if (line.substr(0, PREFIX_SCENE.size()) != PREFIX_SCENE) {
                throw invalid_argument(line);
            }
            scene_title = trim(line.substr(PREFIX_SCENE.size()));
        }
        else {
            // Read fragment file
            _scriptConfig.push_back({scene_title, {}});
            string fragFileName = dir + trim(line);
            ifstream fragFile(fragFileName);
            if (!fragFile) {
                cout << "Can't open character file " << fragFileName << endl;
                continue;
            }
            _readFragConfig(fragFile, _scriptConfig.back().chars, dir);

            // Refresh scene title
            scene_title = "";

            // Calculate pair-of-frags
            size_t sizeThisFrag = _scriptConfig.back().chars.size();
            biggestPairFrags = max(biggestPairFrags, sizeLastFrag+sizeThisFrag);
            sizeLastFrag = sizeThisFrag;
        }
    }

    for (auto &p : _scriptConfig) {
        cout << "[" << p.title << "]" << endl;
        for(auto &s : p.chars) {
            cout << "  " << s.first << " " << s.second << endl;
        }
    }
    cout << biggestPairFrags << endl;

    return biggestPairFrags;
}

bool Director::_readFragConfig(ifstream &fragFile, 
            std::list<tCharConfig> &chars, string dir) {
	list<Player> players;
    // Read play file
    string line;
    while (getline(fragFile, line))
    {
        line = trim(line);
        if (line.empty())
            continue;
        string characterName;
        string inputFileName;
        if (istringstream(line) >> characterName >> inputFileName)
        {
            chars.push_back({characterName, dir+inputFileName});
        }
    }
    return true;
}

void Director::_recruit(size_t numPlayers) {
    // May throw std::bad_alloc or any exception thrown by the constructor 
    _play = make_shared<Play>(_scriptConfig);
    _players.clear();
    for (size_t i = 0; i < numPlayers; i++) {
        _players.push_back(make_shared<Player>(_play.get(), this));
        cout << "Thread " << i << endl;
    }
}

/*
void Director::start() {
    _itNowScene = _scriptConfig.begin();
    _itNowFrag = _itNowScene->second.begin();
}
*/

bool Director::cue() {
	if (_play->distributeEnded()) {
        return true;
    }

    Player *deputy;
    // Wait until there's an idle
	while (!(deputy = atomic_exchange<Player*>(&_idler, NULL))) {
		this_thread::yield();
	}
	cout << "Selected deputy " << deputy << endl;

    tTaskInfo task = _play->getNextTask();
    size_t fragId = task.fragId;
    // First char for me, other chars for followers
    list<tCharConfig>::const_iterator myChar = task.chars.begin();
    list<tCharConfig>::const_iterator newChar = myChar;
    ++newChar;
    Player *follower;
    for(; newChar != task.chars.end(); newChar++) {
		while (!(follower = atomic_exchange<Player*>(&_idler, NULL))) {
			this_thread::yield();
		}
		cout << "Selected follower " << follower << endl;
        follower->assign(fragId, newChar->first, newChar->second);
    }

	//deputy->assignSync(fragId, myChar->first, myChar->second);
    //deputy->doEverything();
	deputy->assign(fragId, myChar->first, myChar->second);
    
	return _play->actEnded();
}

void Director::declareIdle(Player *me) {
	Player *empty = NULL;
	while (!atomic_compare_exchange_strong(&_idler, &empty, me) && !_play->actEnded()) {
		empty = NULL;
		this_thread::yield();
	}
	cout << "I'm selected!" << me << endl;
}

