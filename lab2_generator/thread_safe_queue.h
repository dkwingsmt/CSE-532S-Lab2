#ifndef __THREAD_SAFE_QUEUE_H__
#define __THREAD_SAFE_QUEUE_H__

#include <mutex>
#include <queue>

using namespace std;

template<typename T>
class thread_safe_queue
{
	mutable mutex mut;
	queue<T> data_queue;
	condition_variable data_condition;

public:
	thread_safe_queue() {}

	thread_safe_queue<T>(const thread_safe_queue<T>&) = delete;

	const thread_safe_queue &operator=(const thread_safe_queue<T>&) = delete;

	void push(T new_value) {
		lock_guard<mutex> lk(mut);
		data_queue.push(move(new_value));
		data_condition.notify_one();
	}

	void wait_and_pop(T& value) {
		unique_lock<mutex> lk(mut);
		data_condition.wait(lk, [this]{return !data_queue.empty(); });
		value = move(data_queue.front());
		data_queue.pop();
	}

	bool empty() const {
		lock_guard<mutex> lk(mut);
		return data_queue.empty();
	}

};

#endif
