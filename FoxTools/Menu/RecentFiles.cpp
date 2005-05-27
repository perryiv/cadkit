
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  RecentFiles class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Menu/RecentFiles.h"
#include "FoxTools/Menu/Group.h"
#include "FoxTools/Headers/RecentFiles.h"
#include "FoxTools/Headers/MenuCommand.h"
#include "FoxTools/Headers/MenuSeparator.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Predicates/FileExists.h"

#include <list>

using namespace FoxTools;
using namespace FoxTools::Menu;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RecentFiles::RecentFiles ( unsigned int token ) : BaseClass ( token ),
  _recentFiles ( 0x0 ),
  _commands()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RecentFiles::RecentFiles ( const std::string &name, FX::FXObject *target, unsigned int selector, unsigned int token ) : 
  BaseClass ( name, target, selector, token ),
  _recentFiles ( 0x0 ),
  _commands()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RecentFiles::~RecentFiles()
{
  delete _recentFiles;
  for ( CommandList::iterator i = _commands.begin(); i != _commands.end(); ++i )
    delete (*i);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the button.
//
///////////////////////////////////////////////////////////////////////////////

void RecentFiles::clear()
{
  delete _recentFiles;
  _recentFiles = 0x0;
  for ( CommandList::iterator i = _commands.begin(); i != _commands.end(); ++i )
    delete (*i);
  _commands.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the button.
//
///////////////////////////////////////////////////////////////////////////////

void RecentFiles::_build ( FX::FXComposite *parent )
{
  // Return if we are built.
  if ( _recentFiles )
    return;

  // Make new recent-files.
  _recentFiles = new FX::FXRecentFiles;
  _recentFiles->setTarget ( this->target() );
  _recentFiles->setSelector ( this->selector() );

  // Gather the filenames that exist.
  typedef std::list < std::string > FileList;
  FileList files;
  for ( unsigned int i = FXRecentFiles::ID_FILE_1; i <= FXRecentFiles::ID_FILE_10; ++i )
  {
    const unsigned int index ( i - FXRecentFiles::ID_FILE_1 + 1 );
    std::string file ( _recentFiles->getFile ( index ).text() );
    if ( Usul::Predicates::FileExists() ( file ) )
      files.push_back ( file );
  }

  // Initialize because we rebuild below.
  _recentFiles->clear();

  // Loop in reverse order and add files.
  for ( FileList::reverse_iterator i = files.rbegin(); i != files.rend(); ++i )
    _recentFiles->appendFile ( i->c_str() );

  // This separator only shows up of recent-files is not empty.
  FX::FXMenuSeparator *sep = new FX::FXMenuSeparator ( parent );
  sep->setTarget ( _recentFiles );
  sep->setSelector ( FXRecentFiles::ID_ANYFILES );

  // Create a command for each recent file.
  for ( unsigned int i = FXRecentFiles::ID_FILE_1; i <= FXRecentFiles::ID_FILE_10; ++i )
  {
    _commands.push_back ( new FX::FXMenuCommand ( parent, 0x0, 0x0, _recentFiles, i ) );
  }
  _commands.push_back ( new FX::FXMenuCommand ( parent, this->name().c_str(), NULL, _recentFiles, FXRecentFiles::ID_CLEAR ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a file.
//
///////////////////////////////////////////////////////////////////////////////

void RecentFiles::appendFile ( const std::string &file )
{
  if ( _recentFiles && !file.empty() )
    _recentFiles->appendFile ( file.c_str() );
}
