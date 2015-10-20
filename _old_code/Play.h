// Team Member: Tong Mu mutong32@gmail.com, Jinxuan Zhao jzhao32@wustl.edu
// [File description]
// class Play, to organize the lines from different characters and 
//      construct a printable score. 
// struct PlayLine, a line from a play.

#ifndef __PLAY_H__
#define __PLAY_H__

#include <fstream>
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <vector>

using namespace std;

// Structured lines of the play, which contains order, the character by which 
//it belongs to and the text.
struct PlayLine {
	unsigned int order;
	string character;
	string text;

	bool operator< (const PlayLine &b) const {
		return order < b.order;
	}
};

// class Play, to organize the lines from different characters and 
//      construct a printable score. 
class Play
{
private:
	mutex reciteMutex;
	condition_variable reciteCv;
	string name;
    size_t counter;

	string currentPlayer;

public:
	Play(string playName) : name(playName), counter(1) {}

	void recite(vector<PlayLine>::const_iterator &line);

};

#endif  // header guard
