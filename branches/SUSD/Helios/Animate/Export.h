
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what ANIMATE_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ANIMATE_DLL_EXPORT_H_
#define _ANIMATE_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_ANIMATE
#   define ANIMATE_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define ANIMATE_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // not _WIN32
# define ANIMATE_EXPORT
#endif

#endif // _ANIMATE_DLL_EXPORT_H_
