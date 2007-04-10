#ifndef __SIMFLOWUTIL_EXPORT_H__
#define __SIMFLOWUTIL_EXPORT_H__

#if defined( _MSC_VER )
	// Turn off VC6 warnings. 
	#pragma warning( disable : 4251 )
	#pragma warning( disable : 4273 )
#endif 

#if defined( _MSC_VER ) || defined( __CYGWIN__ ) || defined( __MINGW32__ )
  # if defined( SIMFLOWUTIL_STATIC )
	#	define SCENE_EXPORT	
	# else
	#	ifdef SIMFLOWUTIL_LIBRARY
	#		define SIMFLOWUTIL_EXPORT __declspec(dllexport)
	#	else
	#		define SIMFLOWUTIL_EXPORT __declspec(dllimport)
	#	endif
	# endif
#else
	# define SIMFLOWUTIL_EXPORT
#endif 

#endif // __SIMFLOWUTIL_EXPORT_H__








