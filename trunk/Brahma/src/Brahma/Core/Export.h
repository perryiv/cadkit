
#ifndef __BRAHMA_CORE_EXPORT_H__
#define __BRAHMA_CORE_EXPORT_H__

  #ifdef _MSC_VER 
    #if defined( BRAHMA_STATIC )
	    #define CORE_EXPORT	
	  #else
	    #ifdef BRAHMA_SHARED
	      #define CORE_EXPORT __declspec(dllexport)
	    #else
	      #define CORE_EXPORT __declspec(dllimport)
	    #endif
	  #endif
  #else
	  #define CORE_EXPORT
  #endif 

#endif // __BRAHMA_CORE_EXPORT_H__