
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what ORACLE_WRAP_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ORACLE_WRAPPER_EXPORT_H_
#define _ORACLE_WRAPPER_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_ORACLE_WRAP
#   define ORACLE_WRAP_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define ORACLE_WRAP_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define ORACLE_WRAP_EXPORT
#endif

#endif // _ORACLE_WRAPPER_EXPORT_H_
