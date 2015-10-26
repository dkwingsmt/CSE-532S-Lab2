#include "common.h"
#include "Director.h"

using namespace std;

int main(int argc, char **argv) {
    if (argc < 2) {
		cout << "Need script file name.\n";
		getchar();
        return 1;
    }
    const char *scriptFileName = argv[1];
    Director director(scriptFileName);
    while (!director.cue());
	cout << "End of main." << endl;
	getchar();
    return 0;
}
