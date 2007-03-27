
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  TrJt2Pf.h: Defines what TR_JT_2_PF_API means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_JUPITER_TO_PERFORMER_LIBRARY_API_H_
#define _CADKIT_JUPITER_TO_PERFORMER_LIBRARY_API_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Here we define what TR_JT_2_PF_API means.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable: 4275) // Turn off this warning, it doesn't apply.
# ifdef _CADKIT_COMPILING_TRANSLATOR_JUPITER_2_PERFORMER_LIBRARY
#   define TR_JT_2_PF_API __declspec(dllexport) // We are compiling this library so the classes are exported.
# else
#   define TR_JT_2_PF_API __declspec(dllimport) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define TR_JT_2_PF_API
#endif


#endif // _CADKIT_JUPITER_TO_PERFORMER_LIBRARY_API_H_
