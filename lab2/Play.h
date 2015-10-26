#ifndef __PLAY_H__
#define __PLAY_H__

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <mutex>
#include <condition_variable>
#include <vector>

// {characterName, inputFileName}
typedef std::pair<std::string, std::string> tCharConfig;

struct tFragConfig {
    std::string title;
    std::list<tCharConfig> chars;
};

// Structured lines of the play, which contains order, the character by which 
//it belongs to and the text.
struct PlayLine {
	size_t order;
    std::string character;
    std::string text;

	bool operator< (const PlayLine &b) const {
		return order < b.order;
	}
};

struct tTaskInfo{
    size_t fragId;
    const std::list<tCharConfig> &chars;
};

// class Play, to organize the lines from different characters and 
//      construct a printable score. 
class Play
{
private:
    std::mutex _reciteMutex;
    std::condition_variable _reciteCv;

    std::string _currentPlayer;

    size_t _lineCounter;
    size_t _sceneFragCounter;
    const std::vector<tFragConfig> &_sceneConfig;
    std::list<tCharConfig>::const_iterator _itNextChar;
    size_t _onStage;
    size_t _sceneFragDistributed;


    // return a - b
    static int _cmpFragLine(size_t aFrag, size_t aLine,
                size_t bFrag, size_t bLine) {
        if (aFrag == bFrag && aLine == bLine)
            return 0;
        else if (aFrag > bFrag || 
                    (aFrag == bFrag && aLine > bLine)) 
            return 1;
        return -1;
    }

public:
	explicit Play(const std::vector<tFragConfig> &fragTitles) : 
        _lineCounter(1), 
        _sceneFragCounter(0),
        _sceneConfig(fragTitles),
        _onStage(0),
        _sceneFragDistributed(0)
    {
        if (_sceneFragCounter != _sceneConfig.size()) {
            std::cout << "Scene [" 
                << _sceneConfig[_sceneFragCounter].title 
                << "]" << std::endl;
        }
    }

	void recite(std::vector<PlayLine>::const_iterator &line,
                size_t fragId);

    void enter(size_t fragId);
    void exit();
    bool distributeEnded() { 
        return _sceneFragDistributed == _sceneConfig.size();
	}
	bool actEnded() {
		return _sceneFragCounter == _sceneConfig.size();
	}

    tTaskInfo getNextTask();

};

#endif  // header guard
