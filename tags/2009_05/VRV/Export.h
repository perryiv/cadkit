
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what VRV_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_DLL_EXPORT_H_
#define _VIRTUAL_REALITY_VIEWER_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_VRV
#   define VRV_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define VRV_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define VRV_EXPORT
#endif

#endif // _VIRTUAL_REALITY_VIEWER_DLL_EXPORT_H_
