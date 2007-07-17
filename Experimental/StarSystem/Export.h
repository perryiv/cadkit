
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what STAR_SYSTEM_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_DLL_EXPORT_H_
#define _STAR_SYSTEM_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_STAR_SYSTEM
#   define STAR_SYSTEM_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define STAR_SYSTEM_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define STAR_SYSTEM_EXPORT
#endif

#endif // _STAR_SYSTEM_DLL_EXPORT_H_
