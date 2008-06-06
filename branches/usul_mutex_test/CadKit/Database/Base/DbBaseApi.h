
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseApi.h: Defines what DB_BASE_API means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_LIBRARY_API_H_
#define _CADKIT_DATABASE_BASE_LIBRARY_API_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Here we define what DB_BASE_API means.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable: 4275) // Turn off this warning, it doesn't apply.
# pragma warning(disable: 4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# ifdef _CADKIT_COMPILING_DATABASE_BASE_LIBRARY
#   define DB_BASE_API __declspec(dllexport) // We are compiling this library so the classes are exported.
# else
#   define DB_BASE_API __declspec(dllimport) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define DB_BASE_API
#endif


#endif // _CADKIT_DATABASE_BASE_LIBRARY_API_H_
