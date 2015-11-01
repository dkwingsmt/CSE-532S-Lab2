#include <iostream>
#include "thread_pool.h"
#include "parser.h"
#include "common.h"

using namespace std;

int main(int argc, char *argv[]) {

	if (argc < 3) {
		cerr << "Usage: " << argv[0] << "<script-to-process> <output-scene-prefix>";
		getchar();
		exit(1);
	}

	const string basepath = dirnameOf(argv[0]);

	{
		thread_pool pool(basepath, argv[2]);

		parser &p = parser::get_parser();
		p.set_base_path(basepath);
		p.set_thread_pool(&pool);
		p.parse(argv[1], argv[2]);

	}   //Scope joins thread_pool threads

	cout << "Completed Successfully :)" << endl;
	cout << "Press any key to continue..." << endl;

	getchar();

	exit(1);

}