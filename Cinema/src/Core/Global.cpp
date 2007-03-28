
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

///////////////////////////////////////////////////////////////////////////////
//
// Load xine plugin in memory. 
//
///////////////////////////////////////////////////////////////////////////////
extern "C" void loadPlugins()
{
  #ifdef _WIN32   
    #ifdef _DEBUG
      osgDB::DynamicLibrary::loadLibrary( "osgdb_xined.dll" );
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
