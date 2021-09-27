#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>
#include <chrono>         // std::chrono::seconds
using namespace std;

// a global variable
std::list<int>myList;

// a global instance of std::mutex to protect global variable
std::mutex myMutex;

void addToList(int max, int interval)
{
	// the access to this function is mutually exclusive
	//std::lock_guard<std::mutex> guard(myMutex);
	myMutex.lock();
	for (int i = 0; i < max; i++) {
		std::cout << "[ " << interval << " i: " << i << " ]\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if ((i % interval) == 0) myList.push_back(i);
	}
	myMutex.unlock();
}

void printList()
{
	// the access to this function is mutually exclusive
	std::lock_guard<std::mutex> guard(myMutex);
	std::cout << "Print List\n"<< myList.size();
	for (auto itr = myList.begin(), end_itr = myList.end(); itr != end_itr; ++itr) {
		cout << *itr << ",";
	}
}

int main()
{
	int max = 100;

	std::thread t1(addToList, max, 1);
	std::thread t2(addToList, max, 10);
	std::this_thread::sleep_for(std::chrono::seconds(10));
	std::thread t3(printList);

	t1.join();
	t2.join();
	t3.join();

	return 0;
}