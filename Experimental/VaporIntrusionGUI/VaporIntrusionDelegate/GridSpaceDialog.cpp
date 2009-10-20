
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

#include "GridSpaceDialog.h"

#include "Usul/Convert/Convert.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GridSpaceDialog::GridSpaceDialog ( QWidget *parent ) : BaseClass ( parent ),
_defaultValue ( 5 )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  std::string defaultValue( Usul::Convert::Type< unsigned int, std::string >::convert( _defaultValue ) );

  // set the text fields with the default value
  xValue->setText( defaultValue.c_str() );
  yValue->setText( defaultValue.c_str() );
  zValue->setText( defaultValue.c_str() );

  // set the default spacing to unit length
  _xSpacing->setText( "1" );
  _ySpacing->setText( "1" );
  _zSpacing->setText( "1" );

  // set the text field for the experiment name
  spaceName->setText( "Untitled" );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GridSpaceDialog::~GridSpaceDialog()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the expirement space name
//
///////////////////////////////////////////////////////////////////////////////

std::string GridSpaceDialog::name()
{
  std::string n ( spaceName->text().toStdString() );

  return n;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the (x) spacing
//
///////////////////////////////////////////////////////////////////////////////

float GridSpaceDialog::s()
{
  std::string value ( _xSpacing->text().toStdString() );

  // s value
  float s ( 1.0f );

  if( value.length() > 0 )
  {
    s = Usul::Convert::Type< std::string, float >::convert( value );
  }

  return s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the (y) spacing
//
///////////////////////////////////////////////////////////////////////////////

float GridSpaceDialog::t()
{
  std::string value ( _ySpacing->text().toStdString() );

  // t value
  float t ( 1.0f );

  if( value.length() > 0 )
  {
    t = Usul::Convert::Type< std::string, float >::convert( value );
  }

  return t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the (z) spacing
//
///////////////////////////////////////////////////////////////////////////////

float GridSpaceDialog::u()
{
  std::string value ( _zSpacing->text().toStdString() );

  // u value
  float u ( 1.0f );

  if( value.length() > 0 )
  {
    u = Usul::Convert::Type< std::string, float >::convert( value );
  }

  return u;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the width value (x)
//
///////////////////////////////////////////////////////////////////////////////

unsigned int GridSpaceDialog::x()
{
  std::string value ( xValue->text().toStdString() );

  // x value
  unsigned int x ( _defaultValue );

  if( value.length() > 0 )
  {
    x = Usul::Convert::Type< std::string, unsigned int >::convert( value );
  }

  return x;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the height value (y)
//
///////////////////////////////////////////////////////////////////////////////

unsigned int GridSpaceDialog::y()
{
  std::string value ( yValue->text().toStdString() );

  // y value
  unsigned int y ( _defaultValue );

  if( value.length() > 0 )
  {
    y = Usul::Convert::Type< std::string, unsigned int >::convert( value );
  }

  return y;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the depth value (z)
//
///////////////////////////////////////////////////////////////////////////////

unsigned int GridSpaceDialog::z()
{
  std::string value ( zValue->text().toStdString() );

  // z value
  unsigned int z ( _defaultValue );

  if( value.length() > 0 )
  {
    z = Usul::Convert::Type< std::string, unsigned int >::convert( value );
  }

  return z;

}

