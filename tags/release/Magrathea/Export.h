
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what OSSIM_PLANET_WRAPPER_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSSIM_OSSIMWRAPPER_EXPORT_H__
#define __OSSIM_OSSIMWRAPPER_EXPORT_H__

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_MAGRATHEA
#   define OSSIM_PLANET_WRAPPER_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define OSSIM_PLANET_WRAPPER_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define OSSIM_PLANET_WRAPPER_EXPORT
#endif

#endif // __OSSIM_OSSIMWRAPPER_EXPORT_H__
