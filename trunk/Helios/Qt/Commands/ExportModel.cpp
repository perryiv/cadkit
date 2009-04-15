
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Commands/ExportModel.h"

#include "Usul/Interfaces/IExportModel.h"
#include "Usul/Interfaces/ISaveFileDialog.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Trace/Trace.h"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_COMMAND ( ExportModel );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ExportModel::ExportModel ( IUnknown *caller ) : BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Export &Model" );
  this->shortcut ( "" );
  this->statusTip ( "Export model of current view" );
  this->toolTip ( "Export model of current view" );
  this->iconPath ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ExportModel::~ExportModel ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the image.
//
///////////////////////////////////////////////////////////////////////////////

void ExportModel::_execute ()
{
  USUL_TRACE_SCOPE;

  // Typedefs.
  typedef Usul::Interfaces::IExportModel::Filters        Filters;
  typedef Usul::Interfaces::ISaveFileDialog::FileResult  FileResult;

  // Get the interfaces
  Usul::Interfaces::IExportModel::QueryPtr modelExport ( Usul::Documents::Manager::instance().activeView() );
  Usul::Interfaces::ISaveFileDialog::QueryPtr saveDialog ( this->caller().get() );

  // If the interfaces are valid...
  if( modelExport.valid() && saveDialog.valid() )
  {
    // Get the filters.
    const Filters filters ( modelExport->filtersWriteModel() );

    // Get the filename.
    FileResult result ( saveDialog->getSaveFileName ( "Export Model", filters ) );
    const std::string &filename ( result.first );

    // Return now if the filename is empty.
    if ( filename.empty() )
      return;

    // Export.
    modelExport->writeModelFile ( filename );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool ExportModel::updateEnable () const
{
  Usul::Interfaces::IExportModel::QueryPtr me ( Usul::Documents::Manager::instance().activeView() );
  return me.valid();
}

