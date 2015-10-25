#include "common.h"
#include "Director.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Need script file name.\n";
        return 1;
    }
    const char *scriptFileName = argv[1];
    Director director(scriptFileName);
    return 0;
}
