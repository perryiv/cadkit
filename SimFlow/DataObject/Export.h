
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATA_OBJECT_EXPORT_H__
#define __DATA_OBJECT_EXPORT_H__

#if defined( _MSC_VER )
	// Turn off VC6 warnings. 
	#pragma warning( disable : 4251 )
	#pragma warning( disable : 4273 )
#endif 

#if defined( _MSC_VER ) || defined( __CYGWIN__ ) || defined( __MINGW32__ )
	# if defined( DATA_OBJECT_LIBRARY_STATIC )
	#	define DATA_OBJECT_EXPORT	
	# else
	#	ifdef DATA_OBJECT_LIBRARY
	#		define DATA_OBJECT_EXPORT __declspec(dllexport)
	#	else
	#		define DATA_OBJECT_EXPORT __declspec(dllimport)
	#	endif
	# endif
#else
	# define DATA_OBJECT_EXPORT
#endif 

#endif // __DATA_OBJECT_EXPORT_H__








