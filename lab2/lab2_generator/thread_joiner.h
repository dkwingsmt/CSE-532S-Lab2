#ifndef __THREAD_JOINER_H__
#define __THREAD_JOINER_H__

#include <thread>
#include <vector>
#include <algorithm>

using namespace std;

class thread_joiner
{
	vector<thread>& threads;

public:
	thread_joiner(vector<thread>& threads) : threads(threads) {}
	~thread_joiner() {
		for_each(threads.begin(), threads.end(), mem_fn(&thread::join));
	}
};

#endif
