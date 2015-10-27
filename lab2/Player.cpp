#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "Director.h"
#include "Player.h"
#include "common.h"

using namespace std;

void Player::read() {
    ifstream charFile(_task.followerTask.charFileName);
    if (!charFile) {
        cout << "unable to open file" 
            << _task.followerTask.charFileName << endl;
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
                                    _task.followerTask.charName, 
                                    text}));
            }
        }
    }
}

void Player::act() {
	sort(_lines.begin(), _lines.end());
    for(vector<PlayLine>::const_iterator playIterator = _lines.begin(); 
                playIterator != _lines.end(); /* Blank */)
        _play->recite(playIterator, _task.followerTask.fragId);
}

void Player::_start() {
    while (!_play->actEnded()) {
        _hasTask = false;
        _director->declareIdle(this);
        {
            unique_lock<mutex> lk(_idleMutex);
            if (!_hasTask)
				_idleCv.wait(lk, [&] { 
                            return _hasTask || _play->actEnded(); });
			if (_play->actEnded()) {
				return;
			}
        }
        if (_task.isLeader) {
            _doLeader();
        }
        else {
            _doFollower();
        }
    }
    cout << "Play ended" << endl;
}

void Player::assignFollowerSync(tFollowerTask task) {
    _task.followerTask = move(task);
    _task.isLeader = false;
}

void Player::assignFollower(tFollowerTask task) {
    {
        lock_guard<mutex> lk(_idleMutex);
        assignFollowerSync(move(task));
        _hasTask = true;
    }
    _idleCv.notify_one();
}

void Player::assignLeader(tLeaderTask task) {
    {
        lock_guard<mutex> lk(_idleMutex);
        _task.leaderTask = move(task);
        _task.isLeader = true;
        _hasTask = true;
    }
    _idleCv.notify_one();
}

void Player::_doLeader() {
    size_t fragId = _task.leaderTask.fragId;
    auto &chars = _task.leaderTask.chars;
    auto myChar = chars.begin();
    auto newChar = myChar;
    ++newChar;
    for(; newChar != chars.end(); newChar++) {
        _director->cue(fragId, *newChar);
    }
    assignFollowerSync({fragId, myChar->first, myChar->second});
    _doFollower();
}
