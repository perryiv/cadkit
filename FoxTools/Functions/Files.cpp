
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  File functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Functions/Files.h"

#include "fxdefs.h"
#include "FXString.h"
#include "FXFile.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Get a list of all files in the given directory that match the pattern.
//  Typical patterns are *.txt, *.*, or *
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Functions::files ( const std::string &dir, const std::string &pattern, std::list<std::string> &fl )
{
  // Get list of plugins.
  FX::FXString *fileList ( 0x0 );
  FX::FXint num ( FX::FXFile::listFiles ( fileList, dir.c_str(), pattern.c_str(), FX::LIST_MATCH_ALL ) );

  // Loop through the files.
  for ( FX::FXint i = 0; i < num; ++i )
    fl.push_back ( dir + "/" + fileList[i].text() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a list of all files in the given directories that match the pattern.
//  Typical patterns are *.txt, *.*, or *
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Functions::files ( const std::list<std::string> dirs, const std::string &pattern, std::list<std::string> &fl )
{
  // Loop through the directories.
  typedef std::list<std::string> Dirs;
  for ( Dirs::const_iterator i = dirs.begin(); i != dirs.end(); ++i )
  {
    // Append the files.
    FoxTools::Functions::files ( *i, pattern, fl );
  }
}
