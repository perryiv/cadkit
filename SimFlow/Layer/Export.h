#ifndef __LAYER_EXPORT_H__
#define __LAYER_EXPORT_H__

#if defined( _MSC_VER )
	// Turn off VC6 warnings. 
	#pragma warning( disable : 4251 )
	#pragma warning( disable : 4273 )
#endif 

#if defined( _MSC_VER ) || defined( __CYGWIN__ ) || defined( __MINGW32__ )
	# if defined( LAYER_LIBRARY_STATIC )
	#	define LAYER_EXPORT	
	# else
	#	ifdef LAYER_LIBRARY
	#		define LAYER_EXPORT __declspec(dllexport)
	#	else
	#		define LAYER_EXPORT __declspec(dllimport)
	#	endif
	# endif
#else
	# define LAYER_EXPORT
#endif 

#endif // __LAYER_EXPORT_H__








