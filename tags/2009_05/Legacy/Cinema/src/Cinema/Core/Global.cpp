
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

#include "osgDB/DynamicLibrary"
#include "osgDB/Registry"

///////////////////////////////////////////////////////////////////////////////
//
// Load xine plugin in memory. 
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>

struct XineLife
{
  XineLife()
  {
		osgDB::Registry::instance()->addFileExtensionAlias ( "mov", "qt" );

    #ifdef _WIN32   
      #ifdef _DEBUG
        osgDB::DynamicLibrary::loadLibrary( "osgdb_qtd.dll" );
      #else
        osgDB::DynamicLibrary::loadLibrary( "osgdb_xine.dll" );
      #endif
    #else
      #ifdef _DEBUG 
        osgDB::DynamicLibrary::loadLibrary( "osgdb_xined.so" );
      #else
        osgDB::DynamicLibrary::loadLibrary( "osgdb_xine.so" );
      #endif
    #endif   
  }
  ~XineLife()
  { 
    #if 0
      #ifdef _WIN32   
        #ifdef _DEBUG
          osgDB::DynamicLibrary::freeLibrary( "osgdb_xined.dll" );
        #else
          osgDB::DynamicLibrary::freeLibrary( "osgdb_xine.dll" );
        #endif
      #else
        #ifdef _DEBUG 
          osgDB::DynamicLibrary::freeLibrary( "osgdb_xined.so" );
        #else
          osgDB::DynamicLibrary::freeLibrary( "osgdb_xine.so" );
        #endif
      #endif   
    #endif
  }
} xineLife;

