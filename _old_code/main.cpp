//Team Member: Tong Mu mutong32@gmail.com, Jinxuan Zhao jzhao32@wustl.edu
// lab0_532.cpp :This function contains the thread function and the main functino. It also has the thread guard object.
//

#include "Play.h"
#include "Player.h"
#include "common.h"

using namespace std;

#define CORRECT_ARGC 2
#define ARG_CONFIG_FILE 1

int main(int argc, char *  argv[])
{
    // Parse command line arguments
	if (argc != CORRECT_ARGC ) {
		cout << "usage: " << argv[0] << " <config file>" << endl;
		return ARGUMENT_ERROR;
	}

	ifstream configFile(argv[ARG_CONFIG_FILE]);
	if (!configFile) {
		cout << "Unable to open file " << argv[ARG_CONFIG_FILE] << endl;
		return FILE_NOT_OPEN;
	}

	string playName;
	while (playName.empty() && getline(configFile, playName));  // empty body
	if (playName.empty()) {
		cout << "No play name available." << endl;
		return NO_PLAY_AVAILABLE;
	}
	cout << "[PlayName: " << playName << "]" << endl;

    const string &&fileDir = dirnameOf(argv[ARG_CONFIG_FILE]);

	list<Player> players;
    // Read play file
	Play play(playName);
    string line;
    while (getline(configFile, line))
    {
        line = trim(line);
        if (line.empty())
            continue;
        string characterName;
        string inputFileName;
        if (istringstream(line) >> characterName >> inputFileName)
        {
            players.push_back(Player(play, ifstream(fileDir + inputFileName), characterName));
        }
    }

    // Guard is unnecessary for fstream closes file upon destruction
    configFile.close();

    for (auto &player : players) {
        player.enter();
    }

    for (auto &player : players) {
        player.exit();
    }

	cout << "Finished." << endl;
	getchar();

	return 0;
}



