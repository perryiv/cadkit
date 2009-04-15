
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Commands/ExportScene.h"

#include "Usul/Interfaces/IExportScene.h"
#include "Usul/Interfaces/ISaveFileDialog.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Trace/Trace.h"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_COMMAND ( ExportScene );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ExportScene::ExportScene ( IUnknown *caller ) : BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Export &Scene" );
  this->shortcut ( "" );
  this->statusTip ( "Export top-level scene of current view" );
  this->toolTip ( "Export top-level scene of current view" );
  this->iconPath ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ExportScene::~ExportScene ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the image.
//
///////////////////////////////////////////////////////////////////////////////

void ExportScene::_execute ()
{
  USUL_TRACE_SCOPE;

  // Typedefs.
  typedef Usul::Interfaces::IExportScene::Filters        Filters;
  typedef Usul::Interfaces::ISaveFileDialog::FileResult  FileResult;

  // Get the interfaces
  Usul::Interfaces::IExportScene::QueryPtr sceneExport ( Usul::Documents::Manager::instance().activeView() );
  Usul::Interfaces::ISaveFileDialog::QueryPtr saveDialog ( this->caller().get() );

  // If the interfaces are valid...
  if( sceneExport.valid() && saveDialog.valid() )
  {
    // Get the filters.
    const Filters filters ( sceneExport->filtersWriteScene() );

    // Get the filename.
    FileResult result ( saveDialog->getSaveFileName ( "Export Scene", filters ) );
    const std::string &filename ( result.first );

    // Return now if the filename is empty.
    if ( filename.empty() )
      return;

    // Export.
    sceneExport->writeSceneFile ( filename );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool ExportScene::updateEnable () const
{
  Usul::Interfaces::IExportScene::QueryPtr me ( Usul::Documents::Manager::instance().activeView() );
  return me.valid();
}

