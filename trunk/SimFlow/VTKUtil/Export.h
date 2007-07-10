#ifndef __OSG_VTK_EXPORT_H__
#define __OSG_VTK_EXPORT_H__

#if defined( _MSC_VER )
	// Turn off VC6 warnings. 
	#pragma warning( disable : 4251 )
	#pragma warning( disable : 4273 )
#endif 

#if defined( _MSC_VER ) || defined( __CYGWIN__ ) || defined( __MINGW32__ )
	#	if defined( OSGVTK_LIBRARY_STATIC )
	#		define OSGVTK_EXPORT	
	#	else
	#		if defined ( _VTKUTIL_LIBRARY )
	#			define OSGVTK_EXPORT __declspec(dllexport)
	#		else
	#			define OSGVTK_EXPORT __declspec(dllexport)
	#		endif
	#	endif
#else
	#	define OSGVTK_EXPORT
#endif 

#endif // __OSG_VTK_EXPORT_H__

