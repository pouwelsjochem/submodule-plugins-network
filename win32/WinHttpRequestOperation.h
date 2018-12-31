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

#ifndef _WinHttpRequestOperation_H_
#define _WinHttpRequestOperation_H_

#include "CoronaLua.h"

#include <windows.h>
#include <WinHttp.h>
#include "WinHttpAsyncRequestSessionData.h"

#include "WinHttpRequestError.h"

#include "WindowsNetworkSupport.h"


/// Class used to send an HTTP request to a server and wait for a response asynchronously.
///
class WinHttpRequestOperation
{
public:
	WinHttpRequestOperation();
	virtual ~WinHttpRequestOperation();

	RequestCanceller* ExecuteRequest( NetworkRequestParameters *requestParams, const std::shared_ptr<WinHttpRequestOperation>& thiz);
	bool IsExecuting();
	void ProcessExecution();
	void RequestAbort();

private:
	/// Stores data needed to perform an asynchronous HTTP request operation.
	/// This object's fields are changed on another thread.
	WinHttpAsyncRequestSessionData fAsyncSession;
	NetworkRequestParameters* fRequestParams;
	NetworkRequestState* fRequestState;

	/// Temp file path and file stream to open temp file that serves as the destination of the
	/// response body, if response body is directed to a file.
	UTF8String fTempDownloadFilePath;
	FILE* fDownloadFileStream;

	/// Set true if this object is in the middle of an HTTP request operation.
	bool fIsExecuting;

	bool Execute();
	void ProcessExecutionUntil(int timeoutInMilliseconds);

	static WinHttpRequestError GetRequestErrorFromWinHttpError(DWORD dwError);
	static UTF8String* WinHttpRequestOperation::GetMessageFromWinHttpError(WinHttpRequestError error);
	static void CALLBACK OnAsyncWinHttpStatusChanged(
				HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
				LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	static wchar_t* CreateUtf16StringFrom(const char* utf8String);
	static void DestroyUtf16String(wchar_t *utf16String);
};

#endif
