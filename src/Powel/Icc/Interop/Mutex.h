#pragma once 

#include <windows.h>

namespace Powel { namespace MultiThreading {
	class Mutex 
	{
	protected:
		//TOSA CRITICAL_SECTION _criticalSection;
	public:
		Mutex()
		{
			//TOSA InitializeCriticalSection(&_criticalSection);
		}

		~Mutex()
		{
			//TOSA DeleteCriticalSection(&_criticalSection);
		}

		void Lock()
		{
			//TOSA EnterCriticalSection(&_criticalSection);
		}

		void Unlock()
		{
			//TOSA LeaveCriticalSection(&_criticalSection);
		}

		bool TryLock()
		{
			//TOSA return TryEnterCriticalSection(&_criticalSection)?true:false;
			return true;
		}
	};
}}