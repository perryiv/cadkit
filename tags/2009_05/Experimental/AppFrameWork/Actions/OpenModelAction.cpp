
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command action for opening an existing model.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Actions/OpenModelAction.h"
#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Core/Program.h"

#include "Usul/Interfaces/GUI/ILoadFileDialog.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Exceptions/Canceled.h"
#include "Usul/Components/Manager.h"

#include <iostream>

using namespace AFW::Actions;

USUL_IMPLEMENT_TYPE_ID ( OpenModelAction );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenModelAction::OpenModelAction() : BaseClass(),
  _names()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenModelAction::OpenModelAction ( const std::string &name ) : BaseClass(),
  _names ( 1, name )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenModelAction::OpenModelAction ( const Names &names ) : BaseClass(),
  _names ( names )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenModelAction::~OpenModelAction()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perform the action.
//
///////////////////////////////////////////////////////////////////////////////

void OpenModelAction::execute ( AFW::Core::Object * )
{
  Guard guard ( this->mutex() );

  // Report errors and clear them.
  std::cout << AFW::Core::Program::instance().errors() << std::flush;

  // See if we already have names.
  if ( _names.empty() )
    this->_askForNames();

  // Make a copy of the names.
  Names names ( _names );

  // Clear our list.
  _names.clear();

  // Open files.
  for ( Names::const_iterator i = names.begin(); i != names.end(); ++i )
  {
    // Get file name.
    std::string file ( *i );

    // Try to open the file.
    this->_open ( file );

    // Report any errors.
    this->_reportErrors ( file );

    // Save in the recent-file list.
    {
      AFW_GUARD_PROGRAM;
      AFW::Core::Program::instance().app()->recentFileAppend ( file );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Report any accumulated errors.
//
///////////////////////////////////////////////////////////////////////////////

void OpenModelAction::_reportErrors ( const std::string &file ) const
{
  Guard guard ( this->mutex() );

  // Return if there there are no errors.
  if ( Usul::Errors::Stack::instance().empty() )
    return;

  // Make the message string.
  std::cout << "The following "
            << ( ( Usul::Errors::Stack::instance().size() > 1 ) ? "errors were" : "error was" )
            << " encountered when attempting to load: "
            << file
            << "\n\n"
            << Usul::Errors::Stack::instance().format ( "    " );

  // Clear the error stack.
  Usul::Errors::Stack::instance().clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ask for file names.
//
///////////////////////////////////////////////////////////////////////////////

void OpenModelAction::_askForNames()
{
  // Local typedefs.
  typedef Usul::Interfaces::ILoadFileDialog ILoadFileDialog;
  typedef ILoadFileDialog::Filenames Filenames;
  typedef ILoadFileDialog::Filters Filters;
  typedef ILoadFileDialog::FilesResult FilesResult;
  typedef Usul::Components::Manager PluginManager;
  typedef Usul::Documents::Manager DocumentManager;

  Guard guard ( this->mutex() );

  // Get interface.
  ILoadFileDialog::QueryPtr dialog ( PluginManager::instance().getInterface ( ILoadFileDialog::IID ) );
  if ( false == dialog.valid() )
    return;

  // Get appropriate filters.
  Filters filters ( DocumentManager::instance().filtersOpen() );

  // Ask for file names.
  FilesResult result ( dialog->getLoadFileNames ( "Open", filters ) );
  _names = result.first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the file.
//
///////////////////////////////////////////////////////////////////////////////

void OpenModelAction::_open ( const std::string &file )
{
  Guard guard ( this->mutex() );

  // Safely...
  try
  {
    // Make sure the file exists.
    if ( false == Usul::Predicates::FileExists() ( file ) )
    {
      std::cout << "Input file does not exist: " << file << Usul::Resources::TextWindow::endl;
      return;
    }

    // Feedback.
    std::cout << "Opening file: " << file << Usul::Resources::TextWindow::endl;

    // Feedback.
    std::cout << "Done opening: " << file << Usul::Resources::TextWindow::endl;
  }

  // Did the user cancel?
  catch ( const Usul::Exceptions::Canceled & )
  {
    throw;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    Usul::Errors::Stack::instance().push ( e.what() );
    Usul::Errors::Stack::instance().push ( "Error 1750165826: Standard exception caught when opening file: " + file );
  }
}
