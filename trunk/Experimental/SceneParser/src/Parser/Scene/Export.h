
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
  #if defined( PARSER_SCENENODEVISITOR_STATIC )
	  #define PARSER_SCENENODEVISITOR_EXPORT	
	#else
	  #ifdef  PARSER_SCENENODEVISITOR_SHARED
	    #define PARSER_SCENENODEVISITOR_EXPORT __declspec(dllexport)
	  #else
	    #define PARSER_SCENENODEVISITOR_EXPORT __declspec(dllimport)
	  #endif
	#endif
#else
	#define PARSER_SCENENODEVISITOR_EXPORT
#endif 

#endif // __PARSER_SCENENODEVISITOR_H__
