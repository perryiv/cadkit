

#ifndef __BRAHMA_SCENE_EXPORT_H__
#define __BRAHMA_SCENE_EXPORT_H__

#ifdef _MSC_VER 
  #if defined( BRAHMA_STATIC )
	  #define SCENE_EXPORT	
	#else
	  #ifdef BRAHMA_SHARED
	    #define SCENE_EXPORT __declspec(dllexport)
	  #else
	    #define SCENE_EXPORT __declspec(dllimport)
	  #endif
	#endif
#else
	#define SCENE_EXPORT
#endif 

#endif // __BRAHMA_SCENE_EXPORT_H__