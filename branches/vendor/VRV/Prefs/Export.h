
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what VRV_PREFS_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_PREFS_DLL_EXPORT_H_
#define _VRV_PREFS_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_VRV_PREFS
#   define VRV_PREFS_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define VRV_PREFS_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define VRV_PREFS_EXPORT
#endif

#endif // _VRV_PREFS_DLL_EXPORT_H_
