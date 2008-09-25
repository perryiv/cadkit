
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

#include "MaterialContainer.h"
#include "MaterialDialog.h"

#include "QtGui/QScrollArea"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MaterialContainer::MaterialContainer ( QWidget *parent ) : BaseClass ( parent ),
_materials(),
_layout( new QVBoxLayout() ),
_radioButtons( new QButtonGroup ),
_color( 0.5, 0.5, 0.5, 0.5 ),
_name(),
_value()
{
  // Initialize code from Designer.
  this->setupUi ( this );

  // Create a widget to hold the layout
  QWidget *widget ( new QWidget );

  // Create a scroll area to hold the widget
  QScrollArea *scrollArea ( new QScrollArea );

  // Set the layout of the group
  materialsGroup->setLayout ( new QVBoxLayout );

  // Add the scroll area to the group
  materialsGroup->layout()->addWidget ( scrollArea );

  // set the widget of the scroll area
  scrollArea->setWidget ( widget );

  // Allow resize
  scrollArea->setWidgetResizable ( true );

  // ???
  _layout->addStretch();

  // Set the layout of the widget to be the data member
  widget->setLayout ( _layout );

  // Hook up the signal function
  QObject::connect ( _radioButtons, SIGNAL ( buttonClicked ( QAbstractButton* ) ), this, SLOT ( radioClicked( QAbstractButton*) ) );


}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MaterialContainer::~MaterialContainer()
{
  //std::vector< MaterialDialog* > _materials;
  //QVBoxLayout*                   _layout;
  //QButtonGroup*                  _radioButtons;

  /*for( unsigned int i = 0; i < _materials.size(); ++i )
  {
    delete( _materials.at( i ) );
    _materials.at( i ) = 0x0;
  }*/
  _materials.clear();

  delete _radioButtons;
  _radioButtons = 0x0;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Change the material color and set the current radio button
//
///////////////////////////////////////////////////////////////////////////////

std::string MaterialContainer::getCurrentName()
{
  return _name;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Change the material color and set the current radio button
//
///////////////////////////////////////////////////////////////////////////////

std::string MaterialContainer::getCurrentValue()
{
  return _value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the material color and set the current radio button
//
///////////////////////////////////////////////////////////////////////////////

void MaterialContainer::radioClicked( QAbstractButton * button )
{
  QRadioButton* radio ( dynamic_cast< QRadioButton* > ( button ) );

  if( 0x0 == radio )
    return;

  MaterialDialog* material (  _materials[radio] );

  if( 0x0 == material )
    return;

  _color = material->color();
  _name = material->name();
  _value = material->type();
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a material to the container
//
///////////////////////////////////////////////////////////////////////////////

void MaterialContainer::on_addButton_clicked()
{
  MaterialDialog* a ( new MaterialDialog );

  QRadioButton* radio ( new QRadioButton );
  _radioButtons->addButton ( radio );

  QGridLayout *grid ( new QGridLayout );
  grid->addWidget ( radio, 0, 0 );
  grid->addWidget ( a, 0, 1 );

  //_layout->addWidget ( a );
  _layout->insertLayout ( 0, grid );

  _materials[radio] = a ;

  UserData::RefPtr data ( dynamic_cast< UserData* > ( a ) );

  radio->setUserData( 0, data );

  this->update(); // ?
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current material color
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4 MaterialContainer::getCurrentColor()
{
  return _color;
}
