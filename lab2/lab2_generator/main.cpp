#include <iostream>
#include "thread_pool.h"
#include "parser.h"
#include "common.h"

using namespace std;

#define ARGID_SCRIPT 1
#define ARGID_PREFIX 2

int main(int argc, char *argv[]) {

	if (argc < (ARGID_PREFIX+1)) {
		cerr << "Usage: " << argv[0] << " <script-to-process> <output-scene-prefix>";
        return ARGUMENT_ERROR;
	}

	const string basepath = dirnameOf(argv[0]);

	{
		thread_pool pool(basepath, argv[ARGID_PREFIX]);

		parser &p = parser::get_parser();
		p.set_base_path(basepath);
		p.set_thread_pool(&pool);
		p.parse(argv[ARGID_SCRIPT], argv[ARGID_PREFIX]);

	}   //Scope joins thread_pool threads

	cout << "Completed Successfully :)" << endl;

    return 0;
}
