
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Struct for working with fonts.
//
///////////////////////////////////////////////////////////////////////////////

#include "Font.h"
#include "osgText/Font"
#include "osgDB/Registry"
#include "osgDB/FileUtils"
#include <osgDB/ReadFile>


#include <fstream>

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Return the font filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string Font::fontfile ( const std::string &name )
{
#ifdef _WIN32
  return name + ".ttf";
#elif __APPLE__
  return std::string ( "/Library/Fonts/" ) + name + std::string ( ".ttf" ); 
#else
  return std::string ( "fonts/" ) + name + std::string ( ".ttf" );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the default font for the platform.
//
///////////////////////////////////////////////////////////////////////////////

osgText::Font* Font::defaultFont() 
{
  #ifdef __APPLE__
    // get the existing high level path list
    osgDB::FilePathList wFilePathList = osgDB::Registry::instance()->getDataFilePathList();
    
    // add additional paths of interest to the path list
    wFilePathList.push_back( "/Library/Fonts" ); //Look in the Global Fonts
    wFilePathList.push_back( "./fonts" ); //Look for a "fonts" Folder in the working directory
    wFilePathList.push_back( "/System/Library/Fonts" ); //Look in the System Fonts
    
    // re-assign the expanded path list
    osgDB::Registry::instance()->setDataFilePathList( wFilePathList );

    return osgText::readFontFile( "fudd.ttf" );

  #else

    return osgText::readFontFile( "arial.ttf" );
  #endif
}



