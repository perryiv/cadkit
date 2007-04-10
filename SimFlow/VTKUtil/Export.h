
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VTKUTIL_EXPORT_H__
#define __VTKUTIL_EXPORT_H__

#if defined( _MSC_VER )
	// Turn off VC6 warnings. 
	#pragma warning( disable : 4251 )
	#pragma warning( disable : 4273 )
#endif 

#if defined( _MSC_VER ) || defined( __CYGWIN__ ) || defined( __MINGW32__ )
	# if defined( VTKUTIL_LIBRARY_STATIC )
	#	define VTKUTIL_EXPORT	
	# else
	#	if defined( VTKUTIL_LIBRARY )
	#		define VTKUTIL_EXPORT __declspec(dllexport)
	#	else
	#		define VTKUTIL_EXPORT __declspec(dllexport)
	#	endif
	# endif
#else
	# define VTKUTIL_EXPORT
#endif 

#endif // __VTKUTIL_EXPORT_H__








