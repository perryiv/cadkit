
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __PARSER_EXPORT_H__
#define __PARSER_EXPORT_H__

#if defined( _MSC_VER )
	// Turn off VC6 warnings. 
	#pragma warning( disable : 4251 )
	#pragma warning( disable : 4273 )
#endif 

#if defined( _MSC_VER ) || defined( __CYGWIN__ ) || defined( __MINGW32__ )
	# if defined( PARSER_LIBRARY_STATIC )
	#	define PARSER_EXPORT	
	# else
	#	if defined PARSER_LIBRARY
	#		define PARSER_EXPORT __declspec(dllexport)
	#	else
	#		define PARSER_EXPORT __declspec(dllexport)
	#	endif
	# endif
#else
	# define PARSER_EXPORT
#endif 

#endif // __PARSER_EXPORT_H__









