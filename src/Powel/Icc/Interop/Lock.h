#pragma once 

#include <windows.h>
#include "Mutex.h"

namespace Powel { namespace MultiThreading {
	class Lock 
	{
	protected:
		Mutex &mutex;
	public:
		Lock(Mutex &m) : mutex(m)
		{
			mutex.Lock();
		}
		~Lock()
		{
			mutex.Unlock();
		}
	};
}}