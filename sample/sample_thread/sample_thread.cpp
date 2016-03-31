#include<iostream>
#include"..\..\ZTl\ztl_thread.hpp"

using namespace std;
using namespace ztl;

//thread count
int thread_count = 0;

//mutex
Mutex mutex1;

//example thread
void thread_proc()
{
	for (int i = 0; i < 10; ++i)
	{
		synchronized(mutex1)
		{
			for (char c = 'A'; c <= 'Z'; ++c)
			{
				cout << c;
			}
			cout << endl;
		}
	}
	thread_count--;
}

//main
int main()
{
	thread_count = 2;
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_proc, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_proc, 0, 0, 0);
	while (thread_count) Sleep(0);
	getchar();
	return 0;
}