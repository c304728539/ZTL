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
