
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

#include "WindDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WindDialog::WindDialog ( QWidget *parent ) : 
BaseClass ( parent )
{
  //// Initialize code from Designer.
  this->setupUi ( this );  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WindDialog::~WindDialog()
{
  
}



///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void WindDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void WindDialog::_initialize()
{
  // initialize the direction drop down
  directionDropDown->addItem( "R" );  
  directionDropDown->addItem( "L" );

  // initialize the angle drop down
  angleDropDown->addItem( "0" );  
  angleDropDown->addItem( "45" );
  angleDropDown->addItem( "90" );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wind direction
//
///////////////////////////////////////////////////////////////////////////////

std::string WindDialog::direction()
{
  // get the direction
  std::string direction ( directionDropDown->currentText().toStdString() );

  // get the angle
  std::string angle ( angleDropDown->currentText().toStdString() );

  return Usul::Strings::format( direction, angle );
}
  