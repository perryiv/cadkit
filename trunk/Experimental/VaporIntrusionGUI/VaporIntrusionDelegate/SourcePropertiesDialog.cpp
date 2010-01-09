
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

#include "SourcePropertiesDialog.h"

#include "Usul/Strings/Format.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SourcePropertiesDialog::SourcePropertiesDialog ( QWidget *parent ) : 
BaseClass ( parent )
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SourcePropertiesDialog::~SourcePropertiesDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Sources.
//
///////////////////////////////////////////////////////////////////////////////

SourcePropertiesDialog::Source SourcePropertiesDialog::source()
{
   // get the parameters from the text boxes
  //std::string l    ( this->_length->text().toStdString()        );
  //std::string w    ( this->_width->text().toStdString()         );
  //std::string h    ( this->_height->text().toStdString()        );
  //std::string x    ( this->_xpos->text().toStdString()          );
  //std::string y    ( this->_ypos->text().toStdString()          );
  //std::string z    ( this->_zpos->text().toStdString()          );
  //std::string name ( this->_name->text().toStdString()          );

  // get the color
  QColor qc ( _color->color() );

  // convert the color into a vec4
  Usul::Math::Vec4f color ( static_cast< float > ( qc.redF() ), 
                            static_cast< float > ( qc.greenF() ),
                            static_cast< float > ( qc.blueF() ),
                            static_cast< float > ( qc.alphaF() ) );

  //// create a blank chemicals list
  //Chemicals c;

  //// create a Source object
  //Source s ( l, w, h, x, y, z, name, c );
  //s.color = color;

  //// set the internal source
  //_source = s;

  _source.color = color;

  return _source;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Sources.
//
///////////////////////////////////////////////////////////////////////////////

void SourcePropertiesDialog::source( Source s )
{
  _source = s; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void SourcePropertiesDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialization function.
//
///////////////////////////////////////////////////////////////////////////////

void SourcePropertiesDialog::_initialize()
{
  // get the source
  Source s ( _source );

  // set the parameters from the text boxes
  this->_length->setText( s.l.c_str() );
  this->_width->setText( s.w.c_str() );
  this->_height->setText( s.h.c_str() );
  this->_xpos->setText( s.x.c_str() );
  this->_ypos->setText( s.y.c_str() );
  this->_zpos->setText( s.z.c_str() );
  this->_name->setText( s.name.c_str() );
 
}


  