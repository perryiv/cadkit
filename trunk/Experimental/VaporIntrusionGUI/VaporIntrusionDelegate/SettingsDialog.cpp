
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

#include "SettingsDialog.h"

#include "Usul/Strings/Format.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Math/MinMax.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SettingsDialog::SettingsDialog ( QWidget *parent ) : 
BaseClass ( parent )
{
  // Initialize code from Designer.
  this->setupUi ( this );
  

  //QStringList titles;

  //titles.push_back( "Start" );
  //titles.push_back( "End" );
  //titles.push_back( "Width" );

  //_cracksTable->setHorizontalHeaderLabels( titles );

  //initialize the table view
  this->initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SettingsDialog::~SettingsDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize (public)
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::initialize()
{
  // make sure there are enough colors in the vector
  // to fill the color dialogs
  if( _colors.size() < 4 )
  {
    return;
  }

  // set the building color
  bColor->color( this->_convertColor( _colors.at( 0 ) ) );

  // set the foundation color
  fColor->color( this->_convertColor( _colors.at( 1 ) ) );

  // set the crack color
  cColor->color( this->_convertColor( _colors.at( 2 ) ) );

  // set the grid color
  gColor->color( this->_convertColor( _colors.at( 3 ) ) );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalize
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::finalize()
{
  // clear out the colors
  _colors.clear();

  // set the building color
  _colors.push_back( this->_convertColor( bColor->color() ) );

  // set the building color
  _colors.push_back( this->_convertColor( fColor->color() ) );

  // set the building color
  _colors.push_back( this->_convertColor( cColor->color() ) );

  // set the building color
  _colors.push_back( this->_convertColor( gColor->color() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the colors
//
///////////////////////////////////////////////////////////////////////////////

SettingsDialog::ColorVec SettingsDialog::colors()
{
  return _colors;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the colors
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::colors( ColorVec cv )
{
  _colors = cv;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert an Usul Vec4f to a QColor
//
///////////////////////////////////////////////////////////////////////////////

QColor SettingsDialog::_convertColor ( Usul::Math::Vec4f c )
{
  // get the red channel
  int red ( static_cast< int > ( c[0] * 255.0f ) );

  // get the green channel
  int green ( static_cast< int > ( c[1] * 255.0f ) );

  // get the blue channel
  int blue ( static_cast< int > ( c[2] * 255.0f ) );

  // get the alpha channel
  int alpha ( static_cast< int > ( c[3] * 255.0f ) );
  
  // create the QColor
  QColor qc ( red, green, blue, alpha );

  return qc;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert an Usul Vec4f to a QColor
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f SettingsDialog::_convertColor ( QColor c )
{
  Usul::Math::Vec4f color ( static_cast< float > ( c.redF() ), 
                            static_cast< float > ( c.greenF() ),
                            static_cast< float > ( c.blueF() ),
                            static_cast< float > ( c.alphaF() ) );

  return color;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Convert an Usul Vec4f to a QColor
//
///////////////////////////////////////////////////////////////////////////////

void  SettingsDialog::on_bSlider_sliderMoved( int value )
{
  // convert the input value to a float
  float fValue ( static_cast< float > ( value ) / 100.0f );

  // conver the float to a string
  std::string sValue ( Usul::Strings::format( fValue ) );

  // update the corresponding edit field
  bTrans->setText( sValue.c_str() );

}

///////////////////////////////////////////////////////////////////////////////
//
//  Convert an Usul Vec4f to a QColor
//
///////////////////////////////////////////////////////////////////////////////


void  SettingsDialog::on_fSlider_sliderMoved( int value )
{
  // convert the input value to a float
  float fValue ( static_cast< float > ( value ) / 100.0f );

  // conver the float to a string
  std::string sValue ( Usul::Strings::format( fValue ) );

  // update the corresponding edit field
  fTrans->setText( sValue.c_str() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert an Usul Vec4f to a QColor
//
///////////////////////////////////////////////////////////////////////////////

void  SettingsDialog::on_cSlider_sliderMoved( int value )
{
  // convert the input value to a float
  float fValue ( static_cast< float > ( value ) / 100.0f );

  // conver the float to a string
  std::string sValue ( Usul::Strings::format( fValue ) );

  // update the corresponding edit field
  cTrans->setText( sValue.c_str() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert an Usul Vec4f to a QColor
//
///////////////////////////////////////////////////////////////////////////////

void  SettingsDialog::on_gSlider_sliderMoved( int value )
{
  // convert the input value to a float
  float fValue ( static_cast< float > ( value ) / 100.0f );

  // conver the float to a string
  std::string sValue ( Usul::Strings::format( fValue ) );

  // update the corresponding edit field
  gTrans->setText( sValue.c_str() );

}

///////////////////////////////////////////////////////////////////////////////
//
//  Convert an Usul Vec4f to a QColor
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::on_bTrans_editingFinished()
{
  // convert the value in the field to a std::string
  std::string sValue ( bTrans->text().toStdString() );

  // convert the value to a float
  float fValue ( Usul::Convert::Type< std::string, float >::convert( sValue ) );

  // clamp the value
  fValue = Usul::Math::clamp ( fValue, 0.0f, 1.0f );

  // reset the text value
  bTrans->setText( Usul::Strings::format( fValue ).c_str() );

  // multiply the value by 100
  fValue *= 100.0f;

  // convert the value to an integer
  int iValue ( static_cast< int > ( fValue ) );

  // set the slider
  bSlider->setValue( iValue );

  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert an Usul Vec4f to a QColor
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::on_fTrans_editingFinished()
{
  // convert the value in the field to a std::string
  std::string sValue ( fTrans->text().toStdString() );

  // convert the value to a float
  float fValue ( Usul::Convert::Type< std::string, float >::convert( sValue ) );

  // clamp the value
  fValue = Usul::Math::clamp ( fValue, 0.0f, 1.0f );

  // reset the text value
  fTrans->setText( Usul::Strings::format( fValue ).c_str() );

  // multiply the value by 100
  fValue *= 100.0f;

  // convert the value to an integer
  int iValue ( static_cast< int > ( fValue ) );

  // set the slider
  fSlider->setValue( iValue );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert an Usul Vec4f to a QColor
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::on_cTrans_editingFinished()
{
  // convert the value in the field to a std::string
  std::string sValue ( cTrans->text().toStdString() );

  // convert the value to a float
  float fValue ( Usul::Convert::Type< std::string, float >::convert( sValue ) );

  // clamp the value
  fValue = Usul::Math::clamp ( fValue, 0.0f, 1.0f );

  // reset the text value
  cTrans->setText( Usul::Strings::format( fValue ).c_str() );

  // multiply the value by 100
  fValue *= 100.0f;

  // convert the value to an integer
  int iValue ( static_cast< int > ( fValue ) );

  // set the slider
  cSlider->setValue( iValue );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert an Usul Vec4f to a QColor
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::on_gTrans_editingFinished()
{
  // convert the value in the field to a std::string
  std::string sValue ( gTrans->text().toStdString() );

  // convert the value to a float
  float fValue ( Usul::Convert::Type< std::string, float >::convert( sValue ) );

  // clamp the value
  fValue = Usul::Math::clamp ( fValue, 0.0f, 1.0f );

  // reset the text value
  gTrans->setText( Usul::Strings::format( fValue ).c_str() );

  // multiply the value by 100
  fValue *= 100.0f;

  // convert the value to an integer
  int iValue ( static_cast< int > ( fValue ) );

  // set the slider
  gSlider->setValue( iValue );
}