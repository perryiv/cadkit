
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

#include "Usul/Interfaces/ILoadFileDialog.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Resources/TextWindow.h"

#include <iostream>

using namespace AFW::Actions;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenModelAction::OpenModelAction() : BaseClass()
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
  // Local typedefs.
  typedef Usul::Interfaces::ILoadFileDialog ILoadFileDialog;
  typedef ILoadFileDialog::Filenames Filenames;
  typedef ILoadFileDialog::Filters Filters;
  typedef ILoadFileDialog::FilesResult FilesResult;

  // Get interface.
  Usul::Interfaces::ILoadFileDialog::QueryPtr dialog ( AFW::Core::Application::instance().guiServer() );
  if ( false == dialog.valid() )
    return;

  // Get appropriate filters.
  Filters filters ( Usul::Documents::Manager::instance().filtersOpen() );

  // Ask for file names.
  FilesResult result ( dialog->getLoadFileNames ( "Open", filters ) );
  const Filenames &names ( result.first );

  // Open files.
  for ( Filenames::const_iterator i = names.begin(); i != names.end(); ++i )
    this->_open ( *i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the file.
//
///////////////////////////////////////////////////////////////////////////////

void OpenModelAction::_open ( const std::string &name )
{
  // Feedback.
  std::cout << "Opening file: " << name << Usul::Resources::TextWindow::endl;

  // Feedback.
  std::cout << "Done opening: " << name << Usul::Resources::TextWindow::endl;
}
