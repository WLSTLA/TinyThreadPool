#ifndef THREADPOOL_H
#define THREADPOOL_H

#include"Task.h"
#include<condition_variable>
#include<thread>
#include<iostream>
#include<atomic>
#include<vector>
#include<future>
#include<memory>
#include<windows.h>


class ThreadPool {
public:
	ThreadPool(size_t ThreadNums);
	~ThreadPool();

	void StartWork();
	void Run();

	template<class Fn, class ...ParamTypes>
	auto AddTask(Fn&& functor, ParamTypes&&... params)->std::future<typename std::result_of<Fn(ParamTypes...)>::type>
	{
		using ResType = decltype(functor(params...));
		auto sharedPtr = std::make_shared<std::packaged_task<ResType()>>(
			std::bind(std::forward<Fn>(functor), std::forward<ParamTypes>(params)...)
			);   
		_TaskQueue->ApeendTask(Task([sharedPtr]() {
			(*sharedPtr)();
			}));
		_EmptyCond.notify_one();   
		std::future<ResType> future = sharedPtr->get_future();
		return future;
	}

	void ShutDown();

protected:
	inline bool IsEmptyTask() {
		return _TaskQueue->IsEmpty();
	}
public:
	ThreadPool(const ThreadPool& pool) = delete;
	ThreadPool operator=(const ThreadPool& pool) = delete;
private:
	TaskQueue* _TaskQueue;
	std::vector<std::thread> _ThreadsVector;
	std::atomic<unsigned int> _ThreadNums;	//Ïß³ÌÊý
	std::condition_variable _EmptyCond;
	std::atomic<bool> _IsRun;
	std::mutex _PoolMutex;
};

#endif // !THREADPOOL_H
