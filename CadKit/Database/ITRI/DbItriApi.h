
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbItriApi.h: Defines what DB_ITRI_API means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_ITRI_LIBRARY_API_H_
#define _CADKIT_DATABASE_ITRI_LIBRARY_API_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Here we define what DB_ITRI_API means.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable: 4275) // Turn off this warning, it doesn't apply.
# ifdef _CADKIT_COMPILING_DATABASE_ITRI_LIBRARY
#   define DB_ITRI_API __declspec(dllexport) // We are compiling this library so the classes are exported.
# else
#   define DB_ITRI_API __declspec(dllimport) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define DB_ITRI_API
#endif


#endif // _CADKIT_DATABASE_ITRI_LIBRARY_API_H_
