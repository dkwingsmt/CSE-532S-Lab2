#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "Director.h"
#include "Player.h"
#include "common.h"

using namespace std;

void Player::read() {
    ifstream charFile(_charFileName);
    if (!charFile) {
        cout << "unable to open file" << _charFileName << endl;
        return;
    }

    string line;
    _lines.clear();
    while (getline(charFile, line)) {
        istringstream iss(line);
        size_t lineNumber;
        string text;

        if (iss >> lineNumber) {
            if (getline(iss, text)) {
                trim(text);
                if (!text.empty())
                    _lines.push_back(PlayLine({
                                    lineNumber, 
                                    _charName, 
                                    text}));
            }
        }
    }
}

void Player::act() {
	sort(_lines.begin(), _lines.end());
    for(vector<PlayLine>::const_iterator playIterator = _lines.begin(); 
                playIterator != _lines.end(); /* Blank */)
        _play->recite(playIterator, _fragId);
}

void Player::assignSync(size_t fragId, 
                std::string charName, 
                std::string charFileName) {
    _fragId = fragId;
    _charName = charName;
    _charFileName = charFileName;
}

void Player::assign(size_t fragId, 
                std::string charName, 
                std::string charFileName) {
    {
        lock_guard<mutex> lk(_idleMutex);
        assignSync(fragId, charName, charFileName);
        _hasTask = true;
    }
    _idleCv.notify_one();
}

void Player::_start() {
    while (!_play->actEnded()) {
        _hasTask = false;
        _director->declareIdle(this);
        {
            unique_lock<mutex> lk(_idleMutex);
            if (!_hasTask)
				_idleCv.wait(lk, [&] { return _hasTask || _play->actEnded(); });
			if (_play->actEnded()) {
				return;
			}
        }
        doEverything();
    }
    cout << "Play ended" << endl;
}
