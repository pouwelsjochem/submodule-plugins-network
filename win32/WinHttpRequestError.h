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

#ifndef _WinHttpRequestError_H_
#define _WinHttpRequestError_H_


/// Indicates the error that might have occurred during an HTTP requestion operation.
enum WinHttpRequestError
{
	/// No error has occurred.
	kWinHttpRequestErrorNone,

	/// Unable to connect to the server.
	kWinHttpRequestErrorConnectionFailure,

	/// The HTTP request was aborted on the local machine.
	kWinHttpRequestErrorAborted,

	/// The server failed to respond in time.
	kWinHttpRequestErrorTimedOut,

	/// Could not connect because the URL was invalid.
	kWinHttpRequestErrorInvalidUrl,

	/// The server requires an certificate authentication from the client.
	kWinHttpRequestErrorCertificateRequired,

	/// The server requires login access. Failed to log in to the server.
	kWinHttpRequestErrorLoginFailure,

	/// An internal error on the client side such as out of memory, invalid handles, etc.
	kWinHttpRequestErrorInternal,

	/// An unknown error has occurred during the request.
	kWinHttpRequestErrorUnknown
};

#endif
