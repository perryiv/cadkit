
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CINEMA_CORE_EXPORT_H__
#define __CINEMA_CORE_EXPORT_H__

#if defined( _MSC_VER )
	// Turn off VC6 warnings. 
	#pragma warning( disable : 4251 )
	#pragma warning( disable : 4273 )
#endif 

#if defined( _MSC_VER ) || defined( __CYGWIN__ ) || defined( __MINGW32__ )
	# if defined( CINEMA_CORE_STATIC )
	#	define CINEMA_CORE_EXPORT	
	# else
	#	ifdef CINEMA_CORE_LIBRARY
	#		define CINEMA_CORE_EXPORT __declspec( dllexport )
	#	else
	#		define CINEMA_CORE_EXPORT __declspec( dllimport )
	#	endif
	# endif
#else
	# define CINEMA_CORE_EXPORT
#endif 

#endif // __CINEMA_CORE_EXPORT_H__

