
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines export macro.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_EXPORT_H_
#define _APP_FRAME_WORK_EXPORT_H_


#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Turn off this warning, it doesn't apply.
# pragma warning ( disable : 4251 ) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# ifdef COMPILING_APP_FRAME_WORK
#   define APP_FRAME_WORK_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define APP_FRAME_WORK_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define APP_FRAME_WORK_EXPORT
#endif


#endif // _APP_FRAME_WORK_EXPORT_H_
