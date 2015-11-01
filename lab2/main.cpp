#include "common.h"
#include "Director.h"
#include <string>

using namespace std;

int program(int argc, char **argv) {
    if (argc < 2) {
		cout << "Need script file name.\n";
        return 1;
    }

	int numberOfThreads = 0;
	try{
		numberOfThreads = argc > 2 ? stoi(argv[2]) : 0;
	}
	catch (...) {
		cerr << "3rd parameter expected as a lower bound(integer) on the number of threads" << endl;
		return 2;
	}

	bool bOverride = false;

	if (argc > 3) {
		bOverride = strcmp(argv[3], "-override") == 0;
		if (!bOverride) {
			cerr << "4th parameter expected as -override" << endl;
			return 3;
		}
	}

    const char *scriptFileName = argv[1];
    {
        Director director(scriptFileName, numberOfThreads, bOverride);

		while (!director.electDirector());
		while (!director.actEnded()) this_thread::yield();
		
        // director joins players here.
    }
	cout << "End of main." << endl;
    return 0;
}

int main(int argc, char **argv) {
	int return_code = 0;
	try {
		return_code = program(argc, argv);
	} catch (std::exception &e) {
		cerr << e.what() << endl;
		return_code = 100;
	} catch (...) {
		cerr << "Something went wrong" << endl;
		return_code = 100;
	}
	getchar();
	return return_code;
}
