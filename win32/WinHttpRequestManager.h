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

#include "CoronaLua.h"

#ifndef _WinHttpRequestManager_H_
#define _WinHttpRequestManager_H_

#include "WinTimer.h"

#include "WinHttpRequestOperation.h"

#include "WindowsNetworkSupport.h"

#include <list>
#include <memory>


/// Class supporting concurrent asynchronous HTTP requests.
/// Can set up a LuaResource listener to notify a Lua script the result of this operation.
class WinHttpRequestManager : public WinTimer
{
public:
	WinHttpRequestManager();
	virtual ~WinHttpRequestManager();

	RequestCanceller* SendNetworkRequest( NetworkRequestParameters *requestParams );

	int ActiveRequestCount();
	void ProcessRequests();
	void ProcessRequestsUntil(int timeoutInMilliseconds);
	void AbortAllRequests();
	void OnTimer( );

private:
	/// Typedef for a WinHttpRequestOperation STL list.
	typedef std::list< std::shared_ptr<WinHttpRequestOperation> > WinHttpRequestOperationList;

	/// Collection of HTTP request operations.
	WinHttpRequestOperationList fRequests;

	/// Collection used to temporarily store all requests to be processed by the ProcessRequests() function.
	/// This member variable is to only be used by the ProcessRequests() function.
	WinHttpRequestOperationList fTemporaryRequestList;

	/// Set true if in the middle of processing requests.
	bool fIsProcessingRequests;
};

#endif
