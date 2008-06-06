
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what SIM_FLOW_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SIM_FLOW_DLL_EXPORT_H_
#define _SIM_FLOW_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_SIM_FLOW
#   define SIM_FLOW_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define SIM_FLOW_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define SIM_FLOW_EXPORT
#endif

#endif // _SIM_FLOW_DLL_EXPORT_H_
