
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what MENUKIT_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_DLL_EXPORT_H_
#define _MENUKIT_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_MENUKIT
#   define MENUKIT_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define MENUKIT_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif

#else // _WIN32
# define MENUKIT_EXPORT
#endif

#endif // _MENUKIT_DLL_EXPORT_H_
