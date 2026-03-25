//////////////////////////////////////////////////////////////////////////////
//
// This file is part of the Corona game engine.
// For overview and more information on licensing please refer to README.md 
// Home page: https://github.com/coronalabs/corona
// Contact: support@coronalabs.com
//
//////////////////////////////////////////////////////////////////////////////

#include "WinTimer.h"

#include "WindowsNetworkSupport.h"

namespace
{
	static const wchar_t kWinTimerWindowClassName[] = L"Solar2DNetworkWinTimerWindow";
}

WinTimer::WinTimer( ) 
{
	debug("WinTimer::WinTimer - thread ID: %d", GetCurrentThreadId());

	fWindowHandle = NULL;
	fIntervalInMilliseconds = 10;
	fNextIntervalTimeInTicks = 0;
	fTimer = NULL;
}

WinTimer::~WinTimer()
{
	Stop();
}

bool WinTimer::RegisterWindowClass()
{
	WNDCLASSW windowClass;
	memset(&windowClass, 0, sizeof(windowClass));
	windowClass.lpfnWndProc = WinTimer::MessageWindowProc;
	windowClass.lpszClassName = kWinTimerWindowClassName;

	HMODULE moduleHandle = NULL;
	if (!::GetModuleHandleExW(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			reinterpret_cast<LPCWSTR>(&WinTimer::MessageWindowProc), &moduleHandle))
	{
		moduleHandle = ::GetModuleHandleW(NULL);
	}
	windowClass.hInstance = (HINSTANCE)moduleHandle;

	ATOM classAtom = ::RegisterClassW(&windowClass);
	if ((0 != classAtom) || (ERROR_CLASS_ALREADY_EXISTS == ::GetLastError()))
	{
		return true;
	}
	return false;
}

bool WinTimer::CreateMessageWindow()
{
	if (fWindowHandle)
	{
		return true;
	}

	if (!RegisterWindowClass())
	{
		return false;
	}

	HMODULE moduleHandle = NULL;
	if (!::GetModuleHandleExW(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			reinterpret_cast<LPCWSTR>(&WinTimer::MessageWindowProc), &moduleHandle))
	{
		moduleHandle = ::GetModuleHandleW(NULL);
	}

	fWindowHandle = ::CreateWindowExW(
		0,
		kWinTimerWindowClassName,
		L"",
		0,
		0, 0, 0, 0,
		HWND_MESSAGE,
		NULL,
		(HINSTANCE)moduleHandle,
		this);

	return (NULL != fWindowHandle);
}

void WinTimer::DestroyMessageWindow()
{
	if (fWindowHandle)
	{
		::DestroyWindow(fWindowHandle);
		fWindowHandle = NULL;
	}
}

LRESULT CALLBACK WinTimer::MessageWindowProc(HWND windowHandle, UINT messageId, WPARAM wParam, LPARAM lParam)
{
	if (WM_NCCREATE == messageId)
	{
		CREATESTRUCTW *createStruct = (CREATESTRUCTW*)lParam;
		::SetWindowLongPtrW(windowHandle, GWLP_USERDATA, (LONG_PTR)createStruct->lpCreateParams);
		return TRUE;
	}

	WinTimer *timer = (WinTimer*)::GetWindowLongPtrW(windowHandle, GWLP_USERDATA);
	if (timer)
	{
		if ((WM_TIMER == messageId) && (wParam == timer->fTimer))
		{
			timer->Evaluate();
			return 0;
		}

		if (WM_NCDESTROY == messageId)
		{
			::SetWindowLongPtrW(windowHandle, GWLP_USERDATA, 0);
		}
	}

	return ::DefWindowProcW(windowHandle, messageId, wParam, lParam);
}

// Starts the timer.
void
WinTimer::Start()
{
	// Do not continue if the timer is already running.
	if (IsRunning())
	{
		return;
	}

	if (!CreateMessageWindow())
	{
		return;
	}

	// Start the timer, but with an interval faster than the configured interval.
	// We do this because Windows timers can invoke later than expected.
	// To compensate, we'll schedule when to invoke the timer's callback using "fIntervalEndTimeInTicks".
	fNextIntervalTimeInTicks = ::GetTickCount() + fIntervalInMilliseconds;
	fTimer = ::SetTimer(fWindowHandle, 1, 10, NULL);
	if (0 == fTimer)
	{
		DestroyMessageWindow();
	}
}

// Stops the timer.
void
WinTimer::Stop()
{
	// Do not continue if the timer has already been stopped.
	if (IsRunning() == false)
	{
		return;
	}

	// Stop the timer.
	if (fTimer != 0)
	{
		::KillTimer(fWindowHandle, fTimer);
		fTimer = 0;
	}

	DestroyMessageWindow();
}

// Sets the timer's interval in milliseconds. This can be applied while the timer is running.
// The interval cannot be set less than 10 milliseconds.
void
WinTimer::SetInterval( DWORD milliseconds )
{
	fIntervalInMilliseconds = milliseconds;
}

// Returns true if the timer is currently running.
bool
WinTimer::IsRunning() const
{
	return fTimer != NULL;
}

// Checks if the running timer's interval has elapsed, and if it has, invokes its callback.
// This function is provided because a Windows system timer can trigger late.
// This function will not do anything if the timer is not running.
void
WinTimer::Evaluate()
{
	// Do not continue if the if we haven't reached the scheduled time yet.
	if (CompareTicks(::GetTickCount(), fNextIntervalTimeInTicks) < 0)
	{
		return;
	}

	// Schedule the next interval time.
	for (; CompareTicks(::GetTickCount(), fNextIntervalTimeInTicks) > 0; fNextIntervalTimeInTicks += fIntervalInMilliseconds);

	// Invoke this timer's callback.
	OnTimer();
}


// Computes the delta in milliseconds between two times in milliseconds as returned by
// ::GetTickCount() and represented as DWORD values.  The delta will be negative if x is
// before y, or positive if x is after y.
//
// This logic allows for the time values to wrap, such that you may increment or decrement 
// any such time value, wrapping in either direction, or you may pass in a ::GetTickCount()
// value that has wrapped, and this logic will accommodate that, so long as the time interval
// between x and y is not greater than 2^30 milliseconds (about 12.5 days).
// 
long
WinTimer::GetTickDelta(DWORD x, DWORD y)
{
	// Inspired by http://stackoverflow.com/questions/727918/what-happens-when-gettickcount-wraps
	//
	// Test vectors used:
	//
	// x = 13487231,   y = 13492843,   delta = -5612
	// x = 13492843,   y = 13487231,   delta =  5612
	// x = 4294967173, y = 1111,       delta = -1234
	// x = 1111,       y = 4294967173, delta =  1234
	// x = 0x7fffffff, y = 0x80000000, delta =    -1
	// x = 0x80000000, y = 0x7fffffff, delta =     1
	// x = 0x80000000, y = 0x80000001, delta =    -1
	// x = 0x80000001, y = 0x80000000, delta =     1
	// x = 0x7fffffff, y = 0x80000001, delta =    -2
	// x = 0xffffffff, y = 0x00000001, delta =    -2
	// x = 0x00000000, y = 0xffffffff, delta =     1
	//

	// Compare the given tick values via subtraction (relies on specified behavior of DWORD 
	// and long wrapping, including wrapping on subraction).
	//
	long deltaTime = (long)x - (long)y;
	return deltaTime;
}

// Compares the given tick values returned by ::GetTickCount() using GetTickDelta() (above).
//
// Returns a positive value if "x" is greater than "y".
// Returns zero if "x" is equal to "y".
// Returns a negative value if "x" is less than "y".
//
int
WinTimer::CompareTicks(DWORD x, DWORD y)
{
	long deltaTime = WinTimer::GetTickDelta(x, y);
	if (deltaTime < 0)
	{
		return -1;
	}
	else if (0 == deltaTime)
	{
		return 0;
	}
	return 1;
}

// ----------------------------------------------------------------------------
