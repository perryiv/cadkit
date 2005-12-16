
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) GENERATOR_YEAR, GENERATOR_AUTHOR_NAME
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what GENERATOR_LIBRARY_NAME_UPPER_CASE_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERATOR_LIBRARY_NAME_UPPER_CASE_EXPORT_H_
#define _GENERATOR_LIBRARY_NAME_UPPER_CASE_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_USUL
#   define GENERATOR_LIBRARY_NAME_UPPER_CASE_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define GENERATOR_LIBRARY_NAME_UPPER_CASE_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define GENERATOR_LIBRARY_NAME_UPPER_CASE_EXPORT
#endif

#endif // _GENERATOR_LIBRARY_NAME_UPPER_CASE_EXPORT_H_
