
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
  _objectId ( "" ),
  _label(),
  _labelPosition(),
  _labelColor( 1.0, 1.0, 1.0, 1.0 ),
  _labelSize( 25.0f ),
  _showLabel ( false ),
  _geometry ( static_cast < Usul::Interfaces::IUnknown* > ( 0x0 ) ),
  _dataSource ( static_cast < Usul::Interfaces::IUnknown* > ( 0x0 ) ),
  _firstDate ( boost::date_time::min_date_time ),
  _lastDate ( boost::date_time::max_date_time ),
  _altitudeMode ( CLAMP_TO_GROUND )
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
//  Set the id.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::objectId( const std::string & id )
{
  _objectId = id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the id.
//
///////////////////////////////////////////////////////////////////////////////

const std::string & DataObject::objectId() const
{
  return _objectId;
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

void DataObject::dataSource ( Unknown* c )
{
  _dataSource = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::Unknown* DataObject::dataSource()
{
  return _dataSource.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data source.
//
///////////////////////////////////////////////////////////////////////////////

const DataObject::Unknown* DataObject::dataSource() const
{
  return _dataSource.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node for the label.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* DataObject::_buildLabel( const osg::Vec3& position )
{
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );

  if ( this->showLabel () )
  {
    osg::ref_ptr < osgText::Text > text ( new osgText::Text );

    osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );
    text->setFont( font.get() );

    text->setColor( this->labelColor() );
    text->setPosition ( position );
    text->setAutoRotateToScreen( true );
    text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
    text->setCharacterSize( this->labelSize() );

    text->setText ( this->label() );

    geode->addDrawable( text.get() );
  }

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

void DataObject::preBuildScene( Usul::Interfaces::IUnknown * caller )
{
  Guard guard ( this );
  _preBuiltScene = this->_preBuildScene ( caller );
  this->dirty ( false );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* DataObject::buildScene( Usul::Interfaces::IUnknown* caller )
{
  // Build the scene if we need to.
  if ( this->dirty () )
    this->preBuildScene( caller );

  Guard guard ( this );

  // Switch the pre built scene for what we hand back to osg.
  // This is safer in a multi-threaded environment.
  if( _preBuiltScene.valid() )
  {
    // Get the visibilty state.
    bool visible ( this->visibility () );
    
    _root = _preBuiltScene;
    _preBuiltScene = 0x0;

    // Set the visibility state.
    this->visibility ( visible );
  }

  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visibilty flag.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::visibility ( bool b )
{
  Guard guard ( this );

  if ( _root.valid () )
  {
    unsigned int nodeMask ( b ? 0xffffffff : 0x0 );
    _root->setNodeMask ( nodeMask );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visibilty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool DataObject::visibility ( ) const
{
  Guard guard ( this );
  return _root.valid () ? ( _root->getNodeMask () != 0x0 ) : ( _preBuiltScene.valid() ? _preBuiltScene->getNodeMask() != 0x0 : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the first date.
//
///////////////////////////////////////////////////////////////////////////////

const Minerva::Core::Animate::Date& DataObject::firstDate() const
{
  return _firstDate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the first date.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::firstDate( const Minerva::Core::Animate::Date& date )
{
  _firstDate = date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last date.
//
///////////////////////////////////////////////////////////////////////////////

const Minerva::Core::Animate::Date& DataObject::lastDate() const
{
  return _lastDate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the last date.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::lastDate( const Minerva::Core::Animate::Date& date )
{
  _lastDate = date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the altitude mode.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::altitudeMode ( AltitudeMode mode )
{
  _altitudeMode = mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the altitude mode.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::AltitudeMode DataObject::altitudeMode () const
{
  return _altitudeMode;
}
