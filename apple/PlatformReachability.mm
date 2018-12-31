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


#include "PlatformReachability.h"

#include "AppleNetworkSupport.h"


// ----------------------------------------------------------------------------
	
const char PlatformReachability::kReachabilityListenerEvent[] = "networkStatus";

// ----------------------------------------------------------------------------

PlatformReachability::PlatformReachability( lua_State* L, PlatformReachabilityType type, const char* address)
    : fListenerRef( nil ), fLuaState(L)
{
	debug(@"Creating platform reachability for: %s", address);
    fAddress = [[NSString alloc] initWithUTF8String:address];
}

PlatformReachability::~PlatformReachability()
{
	fListenerRef = nil;
	debug(@"releasing platform reachability for: %@", fAddress);
	[fAddress release];
}

bool
PlatformReachability::IsValid() const
{
	return false;
}

bool
PlatformReachability::IsReachable() const
{
	return false;
}

bool
PlatformReachability::IsConnectionRequired() const
{
	return false;
}

bool
PlatformReachability::IsConnectionOnDemand() const
{
	return false;
}

bool
PlatformReachability::IsInteractionRequired() const
{
	return false;
}

bool
PlatformReachability::IsReachableViaCellular() const
{
	return false;
}

bool
PlatformReachability::IsReachableViaWiFi() const
{
	return false;
}

void PlatformReachability::SetListener( lua_State *L, CoronaLuaRef ref )
{
	if ( nil != fListenerRef )
	{
		CoronaLuaDeleteRef( L, fListenerRef );
	}
	fListenerRef = ref;
}

void PlatformReachability::InvokeCallback() const
{
	if ( nil == fListenerRef )
	{
		debug(@"Not dispatching networkStatus event for address: %@, listener was detached", fAddress);
	}
	else
	{
		debug(@"Dispatching networkStatus event for address: %@", fAddress);

		CoronaLuaNewEvent( fLuaState, kReachabilityListenerEvent );

		lua_pushstring( fLuaState, [fAddress UTF8String] );
		lua_setfield( fLuaState, -2, "address" );

		lua_pushboolean( fLuaState, IsReachable() );
		lua_setfield( fLuaState, -2, "isReachable" );

		lua_pushboolean( fLuaState, IsConnectionRequired() );
		lua_setfield( fLuaState, -2, "isConnectionRequired" );

		lua_pushboolean( fLuaState, IsConnectionOnDemand() );
		lua_setfield( fLuaState, -2, "isConnectionOnDemand" );

		lua_pushboolean( fLuaState, IsInteractionRequired() );
		lua_setfield( fLuaState, -2, "isInteractionRequired" );

		lua_pushboolean( fLuaState, IsReachableViaCellular() );
		lua_setfield( fLuaState, -2, "isReachableViaCellular" );

		lua_pushboolean( fLuaState, IsReachableViaWiFi() );
		lua_setfield( fLuaState, -2, "isReachableViaWiFi" );

		CoronaLuaDispatchEvent( fLuaState, fListenerRef, 0 );
	}
}

// ----------------------------------------------------------------------------
