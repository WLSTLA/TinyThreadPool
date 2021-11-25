#ifndef TASK_H
#define TASK_H

#include<queue>
#include<functional>
#include<mutex>
#include<iostream>
#include<memory>

class Task {
public:
	Task() :_Functor(nullptr) {};
	Task(std::function<void()> InFunctor) :_Functor(InFunctor) {};

	template<class Fn,class ...ParamTypes>
	void Bind(Fn&& Infunc, ParamTypes&&...params) {
		_Functor = std::bind(Infunc, params...);
	}
	void Execute() {
		_Functor();
	}

private:
	std::function<void()> _Functor;
};


class TaskQueue {
public:
	void ApeendTask(const Task& task) {
		std::lock_guard<std::mutex>Lock(_QueueMutex);
		_Queue.emplace(task);
	}

	Task GetTask() {
		Task tempTask = _Queue.front();
		_Queue.pop();
		return tempTask;
	}
	
	inline bool IsEmpty(){
		std::lock_guard<std::mutex>Lock(_QueueMutex);
		return _Queue.empty();
	}

	inline size_t GetTaskNums() {
		std::lock_guard<std::mutex>Lock(_QueueMutex);
		return _Queue.size();
	}
private:
	std::queue<Task> _Queue;
	std::mutex _QueueMutex;
};



#endif // !TASK_H


