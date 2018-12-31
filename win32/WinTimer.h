//////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2018 Corona Labs Inc.
// Contact: support@coronalabs.com
//
// This file is part of the Corona game engine.
//
// Commercial License Usage
// Licensees holding valid commercial Corona licenses may use this file in
// accordance with the commercial license agreement between you and 
// Corona Labs Inc. For licensing terms and conditions please contact
// support@coronalabs.com or visit https://coronalabs.com/com-license
//
// GNU General Public License Usage
// Alternatively, this file may be used under the terms of the GNU General
// Public license version 3. The license is as published by the Free Software
// Foundation and appearing in the file LICENSE.GPL3 included in the packaging
// of this file. Please review the following information to ensure the GNU 
// General Public License requirements will
// be met: https://www.gnu.org/licenses/gpl-3.0.html
//
// For overview and more information on licensing please refer to README.md
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __WinTimer_H__
#define __WinTimer_H__

#include <windows.h>
#include <map>

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
		UINT_PTR	fTimer;
		DWORD		fIntervalInMilliseconds;
		DWORD		fNextIntervalTimeInTicks;

		// Map allowing us to delegate scheduled tasks to object instances
		static std::map<UINT_PTR, WinTimer*> fTimerMap;

		// Static timer proc, used to call object instances
		static void CALLBACK OnTimerElapsed(HWND aHwnd, UINT aMessage, UINT_PTR aTimerId, DWORD aTime); 
};

// ----------------------------------------------------------------------------

#endif // __WinTimer_H__
