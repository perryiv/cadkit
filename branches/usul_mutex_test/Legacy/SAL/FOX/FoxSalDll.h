
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what OSG_FOX_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_SAL_BINDING_IMPORT_EXPORT_H_
#define _FOX_SAL_BINDING_IMPORT_EXPORT_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Here we define what OSG_FOX_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Turn off this warning, it doesn't apply.
# pragma warning ( disable : 4251 ) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# ifdef COMPILING_FOX_SAL
#   define OSG_FOX_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define OSG_FOX_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define OSG_FOX_EXPORT
#endif


#endif // _FOX_SAL_BINDING_IMPORT_EXPORT_H_
