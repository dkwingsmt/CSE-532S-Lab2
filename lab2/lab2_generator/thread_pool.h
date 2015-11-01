#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <vector>
#include <thread>
#include <map>
#include <queue>
#include <atomic>
#include <mutex>
#include <string>
#include <condition_variable>
#include "line.h"
#include "thread_safe_queue.h"
#include "thread_joiner.h"

using namespace std;
class thread_pool
{
	vector<thread> threads;
	thread_joiner  joiner;

	map<string, thread_safe_queue<line>*> character_name_to_queue_map;
	mutex character_name_to_queue_map_lock;
	condition_variable character_map_update_condition;

	vector<pair<string, string>> up_for_grabs; //character_name, file_name (export)
	mutex up_for_grabs_lock;

	const string scene_name;

	const string basepath;

	atomic_int number_of_threads;

	atomic_bool done;

	void remove_local_queue(string character_name);

	void worker_thread();

	bool get_work(pair<string, string> &pair, thread_safe_queue<line> &local_queue);

	void work(string character_name, string file_name, thread_safe_queue<line> *local_queue);

	thread_safe_queue<line>* wait_for_character(string character_name);

public:
	thread_pool(const string &basepath, const string &scene_name) : basepath(basepath), scene_name(scene_name), joiner(threads) {}

	void enter(string character_name, string new_scene_name);

	void exit_all();

	void exit(string character_name, string new_scene_name);
	
	void submit(string character_name, line character_line);

	~thread_pool();
};


#endif
