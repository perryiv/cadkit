
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
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

#include "InputParameterDialog.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

InputParameterDialog::InputParameterDialog ( QWidget *parent ) : 
BaseClass ( parent )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  QStringList titles;
  titles.push_back( "Value" );
  titles.push_back( "Description");

  _inputList->setHorizontalHeaderLabels( titles );
  

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

InputParameterDialog::~InputParameterDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the input list.
//
///////////////////////////////////////////////////////////////////////////////

void InputParameterDialog::_initializeList ()
{
  // Query the active document for IVaporIntrusionGUI
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // get the current input parameters
  Usul::Interfaces::IVaporIntrusionGUI::ParameterList plist ( document->parameters() );

  // set the Grid values
  for( unsigned int i = 0; i < plist.size(); ++i )
  {
    // insert a row
    _inputList->insertRow( i );

    // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
    
    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );

    // insert the columns
    _inputList->setItem( i, 0, item0 );
    _inputList->setItem( i, 1, item1 );

    // set the current item
    _inputList->setCurrentItem( item0 );

    // set the values of the row
    _inputList->item( i, 0 )->setText( plist.at( i ).first.c_str() );
    _inputList->item( i, 1 )->setText( plist.at( i ).second.c_str() );
  }
}

