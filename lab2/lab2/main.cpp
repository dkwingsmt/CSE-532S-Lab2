#include "common.h"
#include "Director.h"
#include <string>

using namespace std;

#define ARGID_SCRIPT    1
#define ARGID_NTHRD     2
#define ARGID_OVERRIDE  3

const char *USAGE = "Usage: lab2.exe <configuration_file_path> <minimum-#-of-player> [-override]";

int program(int argc, char **argv) {
    if (argc < (ARGID_SCRIPT+1)) {
		cout << "Need script file name.\n";
		cout << USAGE << endl;
        return ARGUMENT_ERROR;
    }

	int numberOfThreads = 0;
	try{
		numberOfThreads = argc > (ARGID_NTHRD+1) ? stoi(argv[ARGID_NTHRD]) : 0;
	}
	catch (...) {
		cerr << "3rd parameter expected as a lower bound(integer) on the number of threads" << endl;
		cout << USAGE << endl;
		return ARGUMENT_ERROR;
	}

	bool bOverride = false;

	if (argc > (ARGID_OVERRIDE+1)) {
		bOverride = strcmp(argv[ARGID_OVERRIDE], "-override") == 0;
		if (!bOverride) {
			cerr << "4th parameter expected as -override" << endl;
			cout << USAGE << endl;
			return ARGUMENT_ERROR;
		}
	}

    const char *scriptFileName = argv[ARGID_SCRIPT];
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
		return_code = EXCEPTION;
	} catch (...) {
		cerr << "Something went wrong" << endl;
		return_code = EXCEPTION;
	}
	return return_code;
}
