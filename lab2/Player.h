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
    std::mutex _idleMutex;
    std::condition_variable _idleCv;
    size_t _fragId;
    std::string _charName;
    std::string _charFileName;
    //thread _workThread;

    std::vector<PlayLine> _lines;

    bool _ended;
    bool _hasTask;

    void read();

    void act();

    void _start();

public:
    Player(Play *play, Director *director) :
        _play(play), _director(director), _ended(false),
        _hasTask(false)
    {
        std::thread([this] { _start(); }).detach();
    }

    Player(Player &) = delete;
    Player(Player &&tmp) = default;

    void shut_down() {
        {
            std::lock_guard<std::mutex> lk(_idleMutex);
            _ended = true;
        }
        _idleCv.notify_one();
    }

    //~Player();

    // On stage
    void enter() {
        read();
        _play->enter(_fragId);
        act();
    }

    // Off stage
    void exit() {
        _play->exit();
    }

    void assign(size_t fragId, 
                std::string charName, 
                std::string charFileName);

    void assignSync(size_t fragId, 
                std::string charName, 
                std::string charFileName);

    void doEverything() {
        enter();
        exit();
    }

};



#endif
