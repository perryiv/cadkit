
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what SQL_LITE_WRAP_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_EXPORT_H_
#define _SQL_LITE_WRAP_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_SQL_LITE_WRAP
#   define SQL_LITE_WRAP_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define SQL_LITE_WRAP_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define SQL_LITE_WRAP_EXPORT
#endif

#endif // _SQL_LITE_WRAP_EXPORT_H_
