#pragma once
#ifndef ZTL_THREAD_H
#define ZTL_THREAD_H

#ifdef _LINUX

#endif
#ifdef WIN32
#include<Windows.h>
#endif

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

	/*class thread {

	public:
		thread() noexcept;
		template <class Fn, class... Args>
		explicit thread(Fn&& fn, Args&&... args) {
			DWORD tid;
			//CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)fn, &args...,0,&tid)
		}

		thread(const thread&) = delete;
		thread(thread&& x) noexcept;


	};*/
}

#endif
