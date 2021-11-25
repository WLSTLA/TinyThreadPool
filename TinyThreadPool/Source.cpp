#include<iostream>
#include"ThreadPool.h"
using namespace std;

struct obj {
public:
	int testFunc(float val, const char* str) {
		cout << "obj::testFunc,val= " << val << " str=" << str << " Executed by threadid:" << this_thread::get_id() << endl;
		return val;
	}
};

int testFunc(float val, const char* str) {
	cout << "testFunc,val= " << val << " str=" << str << " Executed by threadid:" << this_thread::get_id() << endl;
	return val;
}


int main() {
	obj o;
	Task t;
	t.Bind(&obj::testFunc,&o,10, "unreal");
	

	std::future<int> future1;

	ThreadPool pool(3);
	pool.StartWork();
	this_thread::sleep_for(chrono::seconds(1));
	for (size_t i = 0; i < 200; i++) {
		future1 = pool.AddTask(testFunc, 10, "unreal");
		//this_thread::sleep_for(chrono::milliseconds(900));
	}
	Sleep(5000);
	pool.ShutDown();
	cout << "future get:" << future1.get() << endl;
	return 0;
}