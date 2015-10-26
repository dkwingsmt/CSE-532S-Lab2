#include "thread_pool.h" 
#include <fstream>
#include <algorithm>
#include <memory>
#include <mutex>

#ifndef CHAR_BASE_PATH
#define CHAR_BASE_PATH(character_name, act_name) basepath + character_name + "_" + scene_name + act_name + ".txt"
#endif

using namespace std;
thread_pool::~thread_pool() {
	done = true;
	exit_all();
}

bool thread_pool::get_work(pair<string, string> &pair, thread_safe_queue<line> &local_queue) {
	lock_guard<mutex> lock(up_for_grabs_lock);
	if (!up_for_grabs.empty()) {
		lock_guard<mutex> inner_lock(character_name_to_queue_map_lock);
		pair = up_for_grabs.back();
		up_for_grabs.pop_back();
		character_name_to_queue_map[pair.first] = &local_queue;
		number_of_threads--;
		character_map_update_condition.notify_one();
		return true;
	}
	return false;
}

void thread_pool::remove_local_queue(string character_name) {
	lock_guard<mutex> lock(character_name_to_queue_map_lock);
	character_name_to_queue_map.erase(character_name);
	number_of_threads++;
}

void thread_pool::worker_thread() {

	number_of_threads++;

	while (!done) {
		pair<string, string> work;
		thread_safe_queue<line> local_queue;
		if (get_work(work, local_queue))  {
			this->work(work.first, work.second, &local_queue);
			if (!done)
				remove_local_queue(work.first);
		}
		else
			this_thread::yield();
	}

}

void thread_pool::work(string character_name, string act_name, thread_safe_queue<line> *local_queue) {

	unique_ptr<ofstream> ofs(new ofstream(CHAR_BASE_PATH(character_name, act_name)));
	int lines_written = 0;
	string current_act = act_name;

	while(true) {
		line next_line;
		local_queue->wait_and_pop(next_line);

		if (next_line.exit) {
			if (lines_written == 0) {
				ofs.reset();
				string file_path = CHAR_BASE_PATH(character_name, current_act);
				remove(file_path.c_str());
			}
			return;
		}

		if (next_line.change_act) {
			current_act = next_line.line_string;
			ofs.reset(new ofstream(CHAR_BASE_PATH(character_name, current_act)));
			lines_written = 0;
			continue;
		}

		*ofs << next_line.line_number << " " << next_line.line_string << endl;
		lines_written++;
	}
}

void thread_pool::enter(string character_name, string new_scene_name) {
	{
		lock_guard<mutex> guard(character_name_to_queue_map_lock);

		for_each(character_name_to_queue_map.begin(), character_name_to_queue_map.end(), [&new_scene_name](pair<string, thread_safe_queue<line>*> character_queue_pair){
			character_queue_pair.second->push(get_change_act_line(new_scene_name));
		});
	}

	{
		lock_guard<mutex> guard(up_for_grabs_lock);
		if (number_of_threads == 0) 
			threads.push_back(thread(&thread_pool::worker_thread, this));
		
		up_for_grabs.push_back(pair<string, string>(character_name, new_scene_name));
	}
	
}

void thread_pool::exit(string character_name, string new_scene_name) {
	lock_guard<mutex> lock(character_name_to_queue_map_lock);
	for_each(character_name_to_queue_map.begin(), character_name_to_queue_map.end(), [&character_name, &new_scene_name](pair<string, thread_safe_queue<line>*> character_queue_pair){
		if (character_queue_pair.first == character_name)
			character_queue_pair.second->push(get_exit_line());
		else
			character_queue_pair.second->push(get_change_act_line(new_scene_name));
	});
}

void thread_pool::exit_all() {
	lock_guard<mutex> lock(character_name_to_queue_map_lock);
	for_each(character_name_to_queue_map.begin(), character_name_to_queue_map.end(), [](pair<string, thread_safe_queue<line>*> character_queue_pair){
		character_queue_pair.second->push(get_exit_line());
	});
}

void thread_pool::submit(string character_name, line character_line) {

	thread_safe_queue<line>* queue = wait_for_character(character_name);

	queue->push(character_line);
	
}

thread_safe_queue<line>* thread_pool::wait_for_character(string character_name) {

	unique_lock<mutex> guard(character_name_to_queue_map_lock);
	
	character_map_update_condition.wait(guard, [&character_name, this]{ return character_name_to_queue_map.find(character_name) != character_name_to_queue_map.end(); });
			
	return character_name_to_queue_map[character_name];

}