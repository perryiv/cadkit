
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
#include "Usul/Convert/Convert.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WindDialog::WindDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_min( std::numeric_limits<float>::max() ),
_max( std::numeric_limits<float>::min() )
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
  // initialize the angle drop down
  angleDropDown->addItem( "0" );  
  angleDropDown->addItem( "45" );
  angleDropDown->addItem( "90" );

  // set the min and max
  _minTextBox->setText( Usul::Strings::format( _min ).c_str() );
  _maxTextBox->setText( Usul::Strings::format( _max ).c_str() );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalize
//
///////////////////////////////////////////////////////////////////////////////

void WindDialog::finalize()
{
  // set the wind direction
  _wind = angleDropDown->currentText().toStdString();

  // set the min and max
  _min = Usul::Convert::Type< std::string, float >::convert( _minTextBox->text().toStdString() );
  _max = Usul::Convert::Type< std::string, float >::convert( _maxTextBox->text().toStdString() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wind direction
//
///////////////////////////////////////////////////////////////////////////////

std::string WindDialog::direction()
{
  // get the angle
  std::string angle ( angleDropDown->currentText().toStdString() );

  return angle;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the min and max
//
///////////////////////////////////////////////////////////////////////////////

void WindDialog::minMax( float min, float max )
{
  _min = min;
  _max = max;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the min and max as a std::pair
//
///////////////////////////////////////////////////////////////////////////////

std::pair< float, float > WindDialog::minMax()
{
  return std::pair< float, float >::pair( _min, _max );
}
  