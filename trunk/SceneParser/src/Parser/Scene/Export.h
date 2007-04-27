
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __PARSER_SCENENODEVISITOR_H__
#define __PARSER_SCENENODEVISITOR_H__

#ifdef _MSC_VER 
  #if defined( SCENEPARSER_STATIC )
	  #define SCENE_EXPORT	
	#else
	  #ifdef SCENEPARSER_SHARED
	    #define SCENE_EXPORT __declspec(dllexport)
	  #else
	    #define SCENE_EXPORT __declspec(dllimport)
	  #endif
	#endif
#else
	#define SCENE_EXPORT
#endif 

#endif // __PARSER_SCENENODEVISITOR_H__