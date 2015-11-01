#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>

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

    std::atomic<bool> _ended;

    void _read();
    void _act();
    void _start();

    void _assignFollowerSync(tFollowerTask);

    void _doLeader();
    void _doFollower() {
        _enter();
        _exit();
    }

    // On stage
    void _enter() {
        _read();
        _play->enter(_task.followerTask.fragId);
        _act();
    }

    // Off stage
    void _exit() {
        _play->exit();
    }


    void _join() {
        {
            std::lock_guard<std::mutex> lk(_idleMutex);
            _ended = true;
        }

        _idleCv.notify_one();
		if (_workThread.joinable())
			_workThread.join();
    }

public:
    Player(Play *play, Director *director) :
        _play(play), 
        _director(director), 
        _hasTask(false),
        _ended(false)
    {
		_workThread = std::move(std::thread([this] { _start(); }));
    }

    ~Player() {
        _join();
    }

    void assignFollower(tFollowerTask);
    void assignLeader(tLeaderTask);

};



#endif
