
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what OSG_STL_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_STL_EXPORT_H_
#define _OSG_STL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_OSG_STL
#   define OSG_STL_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define OSG_STL_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define OSG_STL_EXPORT
#endif

#endif // _OSG_STL_EXPORT_H_
