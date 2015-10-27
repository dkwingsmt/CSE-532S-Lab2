#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <string>
#include <vector>
#include <thread>
//#include <iostream>
//
#include "common.h"
#include "Play.h"

class Director;

class Player {

private:
    Play *_play;
    Director *_director;
    std::thread _workThread;
    std::vector<PlayLine> _lines;

    bool _hasTask;
    tPlayerTask _task;
    std::mutex _idleMutex;
    std::condition_variable _idleCv;

    bool _ended;

    void read();
    void act();
    void _start();

    void _doLeader();
    void _doFollower() {
        enter();
        exit();
    }

public:
    Player(Play *play, Director *director) :
        _play(play), 
        _director(director), 
		_workThread([this] { _start(); }),
        _hasTask(false),
        _ended(false)
    {
    }

    void join() {
        {
            std::lock_guard<std::mutex> lk(_idleMutex);
            _ended = true;
        }
        _idleCv.notify_one();
		if (_workThread.joinable())
			_workThread.join();
    }

    //~Player();

    // On stage
    void enter() {
        read();
        _play->enter(_task.followerTask.fragId);
        act();
    }

    // Off stage
    void exit() {
        _play->exit();
    }

    void assignFollower(tFollowerTask);
    void assignFollowerSync(tFollowerTask);
    void assignLeader(tLeaderTask);

};



#endif
