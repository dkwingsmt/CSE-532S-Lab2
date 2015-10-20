//Team Member: Tong Mu mutong32@gmail.com, Jinxuan Zhao jzhao32@wustl.edu
// Definition of corresponding header file.

#include <algorithm>
#include <iostream>
#include "Play.h"

using namespace std;

void Play::recite(vector<PlayLine>::const_iterator &line) {
    unique_lock<mutex> ul(reciteMutex);
    reciteCv.wait(ul, [&]{ return counter >= line->order; });
    if (counter == line->order) {
        if (currentPlayer != line->character) {
            cout << "\n" << line->character << "." << endl;
            currentPlayer = line->character;
        }
        cout << line->text << endl;
        counter++;
    }
    else {
        cerr << "ERROR: Unexpected PlayLine order. [character= " << line->character << 
            " line.order= " << line->order << " play.counter= " << counter << "\n";
    }
    line++;
    ul.unlock();
    reciteCv.notify_all();
}
