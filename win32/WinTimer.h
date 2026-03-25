//////////////////////////////////////////////////////////////////////////////
//
// This file is part of the Corona game engine.
// For overview and more information on licensing please refer to README.md 
// Home page: https://github.com/coronalabs/corona
// Contact: support@coronalabs.com
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __WinTimer_H__
#define __WinTimer_H__

#include <windows.h>

// ----------------------------------------------------------------------------

class WinTimer
{
	public:
		WinTimer( );
		virtual ~WinTimer();

	public:
		virtual void Start();
		virtual void Stop();
		virtual void SetInterval( ULONG milliseconds );
		virtual bool IsRunning() const;
		virtual void Evaluate();
		virtual void OnTimer() = 0; // Pure virtual, derived class must implement

	public:
		static long GetTickDelta(DWORD x, DWORD y);
		static int CompareTicks(DWORD x, DWORD y);

	private:
		static bool RegisterWindowClass();
		bool CreateMessageWindow();
		void DestroyMessageWindow();
		static LRESULT CALLBACK MessageWindowProc(HWND windowHandle, UINT messageId, WPARAM wParam, LPARAM lParam);

	private:
		HWND		fWindowHandle;
		UINT_PTR	fTimer;
		DWORD		fIntervalInMilliseconds;
		DWORD		fNextIntervalTimeInTicks;
};

// ----------------------------------------------------------------------------

#endif // __WinTimer_H__
