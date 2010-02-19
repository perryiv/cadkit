
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

  // make sure the transparencies have the required number of values
  if( _transparencies.size() < 5 )
  {
    return;
  }

  // set the building values
  this->_setValues( _transparencies.at( 0 ), bTrans, bSlider );

  // set the foundation values
  this->_setValues( _transparencies.at( 1 ), fTrans, fSlider );

  // set the crack values
  this->_setValues( _transparencies.at( 2 ), cTrans, cSlider );

  // set the grid values
  this->_setValues( _transparencies.at( 3 ), gTrans, gSlider );

  // set the pressure values
  this->_setValues( _transparencies.at( 4 ), pTrans, pSlider );

  if( _transparencies.size() < 7 )
  {
    return;
  }

  // set the soil values
  this->_setValues( _transparencies.at( 5 ), soilTrans, soilSlider );

  // set the source values
  this->_setValues( _transparencies.at( 6 ), sourceTrans, sourceSlider );

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

  // clear the transparencies
  _transparencies.clear();

  // set the building transparency
  float bT ( Usul::Convert::Type< std::string, float >::convert( bTrans->text().toStdString() ) );
  _transparencies.push_back( bT );

  // set the foundation transparency
  float fT ( Usul::Convert::Type< std::string, float >::convert( fTrans->text().toStdString() ) );
  _transparencies.push_back( fT );

  // set the building transparency
  float cT ( Usul::Convert::Type< std::string, float >::convert( cTrans->text().toStdString() ) );
  _transparencies.push_back( cT );

  // set the building transparency
  float gT ( Usul::Convert::Type< std::string, float >::convert( gTrans->text().toStdString() ) );
  _transparencies.push_back( gT );

  // set the building transparency
  float pT ( Usul::Convert::Type< std::string, float >::convert( pTrans->text().toStdString() ) );
  _transparencies.push_back( pT );

  // set the soil transparency
  float soilT ( Usul::Convert::Type< std::string, float >::convert( soilTrans->text().toStdString() ) ); 
  _transparencies.push_back( soilT );

  // set the soil transparency
  float sourceT ( Usul::Convert::Type< std::string, float >::convert( sourceTrans->text().toStdString() ) ); 
  _transparencies.push_back( sourceT );

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
//  Get the transparencies
//
///////////////////////////////////////////////////////////////////////////////

SettingsDialog::FloatVec SettingsDialog::transparencies()
{
  return _transparencies;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transparencies
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::transparencies( FloatVec fv )
{
  _transparencies = fv;
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
//  Update information when the user interacts with the building slider
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
//  Update information when the user interacts with the foundation slider
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
//  Update information when the user interacts with the crack slider
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
//  Update information when the user interacts with the grid slider
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
//  Update information when the user interacts with the pressure slider
//
///////////////////////////////////////////////////////////////////////////////

void  SettingsDialog::on_pSlider_sliderMoved( int value )
{
  // convert the input value to a float
  float fValue ( static_cast< float > ( value ) / 100.0f );

  // conver the float to a string
  std::string sValue ( Usul::Strings::format( fValue ) );

  // update the corresponding edit field
  pTrans->setText( sValue.c_str() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Update slider and text information when the user had edited the building
//  edit box.
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::on_bTrans_editingFinished()
{
  // convert the value in the field to a std::string
  std::string sValue ( bTrans->text().toStdString() );

  // convert the value to a float
  float fValue ( Usul::Convert::Type< std::string, float >::convert( sValue ) );

  // set the values
  this->_setValues( fValue, bTrans, bSlider );

  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update slider and text information when the user had edited the foundation
//  edit box.r
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::on_fTrans_editingFinished()
{
  // convert the value in the field to a std::string
  std::string sValue ( fTrans->text().toStdString() );

  // convert the value to a float
  float fValue ( Usul::Convert::Type< std::string, float >::convert( sValue ) );

  // set the values
  this->_setValues( fValue, fTrans, fSlider );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update slider and text information when the user had edited the crack
//  edit box.
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::on_cTrans_editingFinished()
{
  // convert the value in the field to a std::string
  std::string sValue ( cTrans->text().toStdString() );

  // convert the value to a float
  float fValue ( Usul::Convert::Type< std::string, float >::convert( sValue ) );

  // set the values
  this->_setValues( fValue, cTrans, cSlider );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update slider and text information when the user had edited the grid
//  edit box.
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::on_gTrans_editingFinished()
{
  // convert the value in the field to a std::string
  std::string sValue ( gTrans->text().toStdString() );

  // convert the value to a float
  float fValue ( Usul::Convert::Type< std::string, float >::convert( sValue ) );

  // set the values
  this->_setValues( fValue, gTrans, gSlider );  
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update slider and text information when the user had edited the pressure
//  edit box.
//
///////////////////////////////////////////////////////////////////////////////

void SettingsDialog::on_pTrans_editingFinished()
{
  // convert the value in the field to a std::string
  std::string sValue ( gTrans->text().toStdString() );

  // convert the value to a float
  float fValue ( Usul::Convert::Type< std::string, float >::convert( sValue ) );

  // set the values
  this->_setValues( fValue, pTrans, pSlider );  
}


void SettingsDialog::_setValues( float value, QLineEdit* edit, QSlider* slider )
{
  // clamp the value
  value = Usul::Math::clamp ( value, 0.0f, 1.0f );

  // reset the text value
  edit->setText( Usul::Strings::format( value ).c_str() );

  // multiply the value by 100
  value *= 100.0f;

  // convert the value to an integer
  int iValue ( static_cast< int > ( value ) );

  // set the slider
  slider->setValue( iValue );
}