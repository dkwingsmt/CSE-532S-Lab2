#include "Director.h"
#include "common.h"

using namespace std;

const string Director::PREFIX_SCENE = "[scene]";

void Director::_readScript(string &scriptFileName) {
    ifstream scriptFile(scriptFileName);
    if (!scriptFile) {
        throw invalid_argument(scriptFileName);
    }
    
    string line;
    string scene_title;
    list<string> fragmentFiles;
    while (getline(scriptFile, line)) {
        if (line[0] == '[') {
            if (line.substr(0, PREFIX_SCENE.size()) != PREFIX_SCENE) {
                throw invalid_argument(line);
            }
            if (fragmentFiles.size()) {
                _scriptConfig.push_back({scene_title, fragmentFiles});
                fragmentFiles.clear();
            }
            scene_title = trim(line.substr(PREFIX_SCENE.size()));
        }
        else {
            if (scene_title.size()) {
                fragmentFiles.push_back(trim(line));
            }
        }
    }
    if (fragmentFiles.size()) {
        _scriptConfig.push_back({scene_title, fragmentFiles});
    }

    for (auto &p : _scriptConfig) {
        cout << p.first << endl;
        for(auto &s : p.second) {
            cout << "  " << s << endl;
        }
    }
}

