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

#ifndef _WinHttpRequestResult_H_
#define _WinHttpRequestResult_H_

#include "WinHttpRequestError.h"
#include "WinString.h"


/// Stores the result of an HTTP request operation such as the response and any error that has occurred.
struct WinHttpRequestResult
{
public:
	/// Indicates the error that has occurred during an HTTP request operation.
	/// Set to None if no error has occurred.
	WinHttpRequestError Error;

	/// The HTTP status code that was received in the response packet from the server.
	/// Set to -1 if a response was not received.
	int StatusCode;

	/// Set to the body of the HTTP response if not downloading to file.
	/// If downloading to file, then this is set to path\file name the response body was written to.
	/// Set to an empty string if an error occurred.
	WinString Response;


	/// Creates a new initialized result object.
	WinHttpRequestResult()
	{
		Error = kWinHttpRequestErrorNone;
		StatusCode = -1;
	}
};


#endif
