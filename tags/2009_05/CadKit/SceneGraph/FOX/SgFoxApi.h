
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgApi.h: Defines what SG_FOX_API means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_FOX_LIBRARY_API_H_
#define _CADKIT_SCENEGRAPH_OPENGL_FOX_LIBRARY_API_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Here we define what SG_FOX_API means.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable: 4275) // Turn off this warning, it doesn't apply.
# pragma warning(disable: 4251) // Turn off this warning, it doesn't apply.
# ifdef _CADKIT_COMPILING_SCENEGRAPH_OPENGL_FOX_LIBRARY
#   define SG_FOX_API __declspec(dllexport) // We are compiling this library so the classes are exported.
# else
#   define SG_FOX_API __declspec(dllimport) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define SG_FOX_API
#endif


#endif // _CADKIT_SCENEGRAPH_OPENGL_FOX_LIBRARY_API_H_
