#include"ThreadPool.h"

ThreadPool::ThreadPool(size_t ThreadNums)
{
	_ThreadNums = ThreadNums;
	_IsRun = false;
	_TaskQueue = new TaskQueue;
}

ThreadPool::~ThreadPool()
{
	if (_TaskQueue) {
		delete _TaskQueue;
		_TaskQueue = nullptr;
	}
}

void ThreadPool::StartWork()
{
	for (size_t i = 0; i < _ThreadNums; ++i) {
		_ThreadsVector.emplace_back(std::thread(&ThreadPool::Run, this));
	}
	_IsRun = true;
}

void ThreadPool::Run()
{
	_PoolMutex.lock();
	std::cout << "threadId:" << std::this_thread::get_id() << " is getting work!\n";
	_PoolMutex.unlock();

	while (_IsRun)
	{
		Task targetTask;
		{
			std::unique_lock<std::mutex>ULock(_PoolMutex);
			//if (this->IsEmptyTask()) {
			//	std::cout << "threadId:" << std::this_thread::get_id() << " wait task(task is empty)...\n";
			//	_EmptyCond.wait(ULock);
			//}
			_EmptyCond.wait(ULock, [this]() {
				return !_IsRun||!this->IsEmptyTask();
				});

			if (!_IsRun) {
				std::cout << "threadId:" << std::this_thread::get_id() << " is getting shutdown...\n";
				return;
			}
			targetTask = std::move(_TaskQueue->GetTask());
		}
		targetTask.Execute();
	}
}



void ThreadPool::ShutDown()
{
	_IsRun = false;
	_EmptyCond.notify_all();
	for (auto& thread : _ThreadsVector) {
		if (thread.joinable()) {
			thread.join();
		}
	}
	if (_TaskQueue) {
		delete _TaskQueue;
		_TaskQueue = nullptr;
	}
}
