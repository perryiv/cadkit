
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Commands/ExportImage.h"

#include "Usul/Interfaces/IExportImage.h"
#include "Usul/Interfaces/ISaveFileDialog.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Trace/Trace.h"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_COMMAND ( ExportImage );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ExportImage::ExportImage ( IUnknown *caller ) : BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Export &Image" );
  this->shortcut ( "" );
  this->statusTip ( "Export Image of current view" );
  this->toolTip ( "Export Image of current view" );
  this->iconPath ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ExportImage::~ExportImage ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the image.
//
///////////////////////////////////////////////////////////////////////////////

void ExportImage::_execute ()
{
  USUL_TRACE_SCOPE;

  // Typedefs.
  typedef Usul::Interfaces::IExportImage::Filters        Filters;
  typedef Usul::Interfaces::ISaveFileDialog::FileResult  FileResult;

  // Get the interfaces
  Usul::Interfaces::IExportImage::QueryPtr imageExport ( Usul::Documents::Manager::instance().activeView() );
  Usul::Interfaces::ISaveFileDialog::QueryPtr saveDialog ( this->caller().get() );

  // If the interfaces are valid...
  if( imageExport.valid() && saveDialog.valid() )
  {
    // Get the filters.
    const Filters filters ( imageExport->filtersWriteImage() );

    // Get the filename.
    FileResult result ( saveDialog->getSaveFileName ( "Export Image", filters ) );
    const std::string &filename ( result.first );

    // Return now if the filename is empty.
    if ( filename.empty() )
      return;

    // Export.
    imageExport->writeImageFile ( filename );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool ExportImage::updateEnable () const
{
  return 0x0 != Usul::Documents::Manager::instance().activeView();
}

