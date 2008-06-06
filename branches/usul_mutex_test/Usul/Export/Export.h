
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what USUL_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DLL_EXPORT_H_
#define _USUL_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_USUL
#   define USUL_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define USUL_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define USUL_EXPORT
#endif

#endif // _USUL_DLL_EXPORT_H_
