
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all data objects.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/DataObjects/DataObject.h"
#include "Minerva/Core/Visitor.h"

#include "OsgTools/Font.h"

#include "osg/Geode"
#include "osgText/Text"
#include "osg/Billboard"

using namespace Minerva::Core::DataObjects;


///////////////////////////////////////////////////////////////////////////////
//
//  Static Member.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::ShapeFactory::Ptr DataObject::_sf ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Return the shape factory.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::ShapeFactory* DataObject::shapeFactory()
{
  if ( 0x0 == _sf.get() )
    _sf = new OsgTools::ShapeFactory;
  return _sf.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::DataObject() :
BaseClass(),
_dirty ( true ),
_renderBin ( osg::StateSet::DEFAULT_BIN ),
_color ( 0.0, 0.0, 0.0, 1.0 ),
_tableName(),
_rowId ( -1 ),
_label(),
_labelPosition(),
_labelColor( 1.0, 1.0, 1.0, 1.0 ),
_labelSize( 25.0f ),
_showLabel ( false ),
_geometry ( static_cast < Usul::Interfaces::IUnknown* > ( 0x0 ) ),
_connection ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::~DataObject()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::accept ( Minerva::Core::Visitor& visitor )
{
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool DataObject::dirty() const
{
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::dirty( bool b )
{
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the render bin.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int DataObject::renderBin() const
{
  return _renderBin;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the render bin.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::renderBin( unsigned int renderBin )
{
  // Only change it if it's different.
  if ( renderBin != _renderBin )
  {
    _renderBin = renderBin;
    this->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4& DataObject::color () const
{
  return _color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::color ( const osg::Vec4& color )
{
  // Set the internal color.
  _color = color;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the table name.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::tableName ( const std::string& name )
{
  _tableName = name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the table name.
//
///////////////////////////////////////////////////////////////////////////////

const std::string & DataObject::tableName () const
{
  return _tableName;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the row id.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::rowId( int id )
{
  _rowId = id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the row id.
//
///////////////////////////////////////////////////////////////////////////////

int DataObject::rowId() const
{
  return _rowId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::label ( const std::string& label )
{
  _label = label;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& DataObject::label () const
{
  return _label;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label position.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::labelPosition ( const osg::Vec3& position )
{
  _labelPosition = position;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label position.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3& DataObject::labelPosition () const
{
  return _labelPosition;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the geometry.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::geometry( Unknown *geometry )
{
  _geometry = geometry;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the geometry.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::Unknown* DataObject::geometry()
{
  return _geometry.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the geometry.
//
///////////////////////////////////////////////////////////////////////////////

const DataObject::Unknown* DataObject::geometry() const
{
  return _geometry.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4& DataObject::labelColor () const
{
  return _labelColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::labelColor ( const osg::Vec4& color )
{
  // Set the lable color.
  _labelColor = color;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label size.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::labelSize( float size )
{
  _labelSize = size;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label size.
//
///////////////////////////////////////////////////////////////////////////////

float DataObject::labelSize() const
{
  return _labelSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the connection.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::connection ( Minerva::Core::DB::Connection* c )
{
  _connection = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::DB::Connection* DataObject::connection()
{
  return _connection.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection.
//
///////////////////////////////////////////////////////////////////////////////

const Minerva::Core::DB::Connection* DataObject::connection() const
{
  return _connection.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node for the label.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* DataObject::_buildLabel()
{
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr < osgText::Text > text ( new osgText::Text );

  text->setFont( OsgTools::Font::defaultFont() );
  text->setColor( this->labelColor() );
  text->setPosition ( this->labelPosition() );
  text->setAutoRotateToScreen( true );
  text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
  text->setCharacterSize( this->labelSize() );

  text->setText ( this->label() );

  geode->addDrawable( text.get() );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag to show the label.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::showLabel ( bool value )
{
  _showLabel = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag to show the label.
//
///////////////////////////////////////////////////////////////////////////////

bool DataObject::showLabel () const
{
  return _showLabel;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::preBuildScene()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this data object transparent?
//
///////////////////////////////////////////////////////////////////////////////

bool DataObject::transparent() const
{
  return 1.0f != this->color().w();
}
