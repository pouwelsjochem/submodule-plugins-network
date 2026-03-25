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
	static const UINT kWinTimerMessageId = WM_APP + 0x31;
}

WinTimer::WinTimer()
{
	debug("WinTimer::WinTimer - thread ID: %d", GetCurrentThreadId());

	fWindowHandle = NULL;
	fThreadHandle = NULL;
	fStopEvent = NULL;
	fIsRunning = 0;
	fTickPending = 0;
	fIntervalInMilliseconds = 10;
	fNextIntervalTimeInTicks = 0;
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
	return ((0 != classAtom) || (ERROR_CLASS_ALREADY_EXISTS == ::GetLastError()));
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

DWORD WINAPI WinTimer::TimerThreadProc(LPVOID context)
{
	WinTimer *timer = (WinTimer*)context;
	timer->RunTimerThread();
	return 0;
}

void WinTimer::RunTimerThread()
{
	while (InterlockedCompareExchange(&fIsRunning, 0, 0) != 0)
	{
		if (fStopEvent && (WAIT_OBJECT_0 == ::WaitForSingleObject(fStopEvent, 10)))
		{
			break;
		}

		if (NULL == fWindowHandle)
		{
			continue;
		}

		if (0 == InterlockedCompareExchange(&fTickPending, 1, 0))
		{
			if (!::PostMessageW(fWindowHandle, kWinTimerMessageId, 0, 0))
			{
				InterlockedExchange(&fTickPending, 0);
			}
		}
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
		if (kWinTimerMessageId == messageId)
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

void WinTimer::Start()
{
	if (IsRunning())
	{
		return;
	}

	if (!CreateMessageWindow())
	{
		return;
	}

	InterlockedExchange(&fTickPending, 0);
	InterlockedExchange(&fIsRunning, 1);
	fNextIntervalTimeInTicks = ::GetTickCount() + fIntervalInMilliseconds;

	fStopEvent = ::CreateEventW(NULL, TRUE, FALSE, NULL);
	if (NULL == fStopEvent)
	{
		InterlockedExchange(&fIsRunning, 0);
		DestroyMessageWindow();
		return;
	}

	fThreadHandle = ::CreateThread(NULL, 0, WinTimer::TimerThreadProc, this, 0, NULL);
	if (NULL == fThreadHandle)
	{
		InterlockedExchange(&fIsRunning, 0);
		::CloseHandle(fStopEvent);
		fStopEvent = NULL;
		DestroyMessageWindow();
	}
}

void WinTimer::Stop()
{
	if (!IsRunning())
	{
		return;
	}

	InterlockedExchange(&fIsRunning, 0);
	InterlockedExchange(&fTickPending, 0);

	if (fStopEvent)
	{
		::SetEvent(fStopEvent);
	}

	if (fThreadHandle)
	{
		::WaitForSingleObject(fThreadHandle, INFINITE);
		::CloseHandle(fThreadHandle);
		fThreadHandle = NULL;
	}

	if (fStopEvent)
	{
		::CloseHandle(fStopEvent);
		fStopEvent = NULL;
	}

	DestroyMessageWindow();
}

void WinTimer::SetInterval(DWORD milliseconds)
{
	fIntervalInMilliseconds = milliseconds;
}

bool WinTimer::IsRunning() const
{
	return (InterlockedCompareExchange(const_cast<LONG*>(&fIsRunning), 0, 0) != 0);
}

void WinTimer::Evaluate()
{
	if (!IsRunning())
	{
		InterlockedExchange(&fTickPending, 0);
		return;
	}

	if (CompareTicks(::GetTickCount(), fNextIntervalTimeInTicks) < 0)
	{
		InterlockedExchange(&fTickPending, 0);
		return;
	}

	for (; CompareTicks(::GetTickCount(), fNextIntervalTimeInTicks) > 0; fNextIntervalTimeInTicks += fIntervalInMilliseconds);

	OnTimer();
	InterlockedExchange(&fTickPending, 0);
}

long WinTimer::GetTickDelta(DWORD x, DWORD y)
{
	long deltaTime = (long)x - (long)y;
	return deltaTime;
}

int WinTimer::CompareTicks(DWORD x, DWORD y)
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

