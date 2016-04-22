#pragma once
#ifndef ZTL_THREAD_H
#define ZTL_THREAD_H

#ifdef _LINUX

#endif
#ifdef WIN32
#include<Windows.h>

#endif
#include"ztl_vector.hpp"
#include<functional>
#include<process.h>
#include"ztl_queue.hpp"
namespace ztl {
	//for Window
	
#ifdef _LINUX

	using CriticalSectionLock = pthread_mutex_t;

#endif

#ifdef WIN32

	using CriticalSectionLock = CRITICAL_SECTION;

#endif


	class Mutex
	{
	public:
		//the default constructor
		Mutex()
		{
#ifdef _LINUX
			pthread_mutex_init(&criticalSection, NULL);
#endif
#ifdef WIN32
			InitializeCriticalSection(&criticalSection);
#endif
		}

		//destructor
		~Mutex()
		{
#ifdef _LINUX
			pthread_mutex_init(&criticalSection, NULL);
#endif
#ifdef WIN32
			DeleteCriticalSection(&criticalSection);
#endif
		}

		//lock
		void lock()
		{
#ifdef _LINUX
			thread_mutex_lock(&criticalSection);
#endif
#ifdef WIN32
			EnterCriticalSection(&criticalSection);
#endif
		}

		//unlock
		void unlock()
		{
#ifdef _LINUX
			pthread_mutex_unlock(&criticalSection, NULL);
#endif
#ifdef WIN32
			LeaveCriticalSection(&criticalSection);
#endif
		}

	private:
		CriticalSectionLock criticalSection;
	};

	class Lock
	{
	public:
		//the default constructor
		Lock(Mutex &mutex) : m_mutex(mutex), m_locked(true)
		{
			mutex.lock();
		}

		//the destructor
		~Lock()
		{
			m_mutex.unlock();
		}

		//report the state of locking when used as a boolean
		operator bool() const
		{
			return m_locked;
		}

		//unlock
		void setUnlock()
		{
			m_locked = false;
		}

	private:
		Mutex &m_mutex;
		bool m_locked;
	}; 
	

	#define synchronized(M)  for(Lock M##_lock = M; M##_lock; M##_lock.setUnlock())

	/*
	class paralleljob {
	private:
		vector<std::thread&> threads;
		vector<std::thread&> waitthreads;
		size_t index = 0;
	public:
		void SetWaits(const vector<std::thread&>& waits) { waitthreads = waits; }
		void SetThreads(const vector<std::thread&>& threads) { this->threads = threads; }
		void wait() { 
			for (auto& th : waitthreads)
				th.join(); 
		}

		std::thread& getone() { 
			return threads[index++];
		}



	};
	
	*/

	class threadi {
		std::function<void()> f;
		std::thread* pt = nullptr;
		HANDLE hSemaphore = 0;
		Mutex overM;
		bool bover = false;
	public:
		threadi(const std::function<void()>& _f): f(_f) {
			hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
		}

		void run() {
			pt = new std::thread(f);
			ReleaseSemaphore(hSemaphore, 1, NULL);
		}

		void over() {
			synchronized(overM) {
				if (!bover)
				{
					WaitForSingleObject(hSemaphore, INFINITE);
					pt->join();
					bover = true;
				}
			}
		}

		~threadi() {
			//std::cout << "threadi deconstruct"<<std::endl;
			if (pt)
			{
				delete pt;
				pt = nullptr;
			}
			CloseHandle(hSemaphore);
		}
	};

	
	template<int n>
	class threadmanage {
	private:
		const unsigned int maxNumParallel = 4;
		queue<threadi*> threadwaiting;
		queue<threadi*> threadrunning;
		Mutex mutexwait;
		bool exit = false;
	public:
		threadmanage() {
			_beginthread(run, 0, this);
		}

		void AddThread(threadi* thread) {
			synchronized(mutexwait) {
				threadwaiting.push(thread);
			}
		}

		void Exit() {
			while (threadrunning.size() != 0 || threadwaiting.size() != 0);
			synchronized(mutexExit) {
				exit = true;
			}
		}
	private:
		static Mutex mutexExit;

		static void run(void* pthis) {
			threadmanage* ptm = (threadmanage*)pthis;
			while(1)
			synchronized(mutexExit){
				if (!ptm->exit)
				{
					if (ptm->threadrunning.size() < ptm->maxNumParallel)
					{
						if (ptm->threadwaiting.size() > 0)
						{
							auto& mutext = ptm->mutexwait;
							synchronized(mutext) {
								ptm->threadwaiting.front()->run();
								ptm->threadrunning.push(ptm->threadwaiting.front());
								ptm->threadwaiting.pop();
							}
						}
						else
						{
							//std::this_thread::sleep_for(std::chrono::milliseconds(1));
							if (ptm->threadrunning.size() > 0)
							{
								ptm->threadrunning.front()->over();
								ptm->threadrunning.pop();
							}
						}
					}
					else // if (ptm->threadrunning.size() > 0)
					{
						ptm->threadrunning.front()->over();
						ptm->threadrunning.pop();
					}
					
				}
				else
					return;
			}
		}
	};
	template<int n>
	Mutex threadmanage<n>::mutexExit;


	/*
	class thread {
	public:
		enum {
			thread_run_now,
			thread_run_hold
		};
	private:
		
	public:
		thread() {

		}

		template<typename _RT, typename... Args>
		thread(_RT(*pf)(Args...), Args... args) {
			auto f = bind(pf, args...);
		}

		void join() {
			//_beginthread()
		}
	};

	*/
}

#endif
