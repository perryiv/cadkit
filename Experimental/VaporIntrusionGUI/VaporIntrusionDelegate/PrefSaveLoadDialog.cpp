
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for files.
//
///////////////////////////////////////////////////////////////////////////////

#include "PrefSaveLoadDialog.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Documents/Manager.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PrefSaveLoadDialog::PrefSaveLoadDialog ( QWidget *parent ) : BaseClass ( parent )
{
  // Initialize code from Designer.
  this->setupUi ( this );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PrefSaveLoadDialog::~PrefSaveLoadDialog()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Save Button has been clicked
//
///////////////////////////////////////////////////////////////////////////////

void PrefSaveLoadDialog::on_saveButton_clicked()
{
  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // get the username
  std::string username ( this->_username->text().toStdString() );

  // tell the document to save the preferences
  document->writeUserPreferences( username );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Load Button has been clicked
//
///////////////////////////////////////////////////////////////////////////////

void PrefSaveLoadDialog::on_loadButton_clicked()
{
  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // get the username
  std::string username ( this->_username->text().toStdString() );

  // tell the document to save the preferences
  document->readUserPreferences( username );
}
