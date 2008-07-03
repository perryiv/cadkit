
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/PostGIS/PolygonLayer.h"
#include "Minerva/Layers/PostGIS/BinaryParser.h"

#include "Minerva/Core/Visitor.h"
#include "Minerva/Core/Geometry/Polygon.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"

#include <algorithm>

//USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_4 ( osg::Vec4 );
//USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_4 ( osg::Vec4 );
//SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( osg::Vec4 );

using namespace Minerva::Layers::PostGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( PolygonLayer, PolygonLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( PolygonLayer );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::PolygonLayer() : BaseClass(), 
  _showInterior( true ),
  _showBorder( false ),
  _borderColor( 0.0, 0.0, 0.0, 1.0 ),
  _borderWidth ( 1.0f )
{
  USUL_TRACE_SCOPE;
  this->name( "PolygonLayer" );
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::PolygonLayer ( const PolygonLayer& layer ) : BaseClass ( layer ),
  _showInterior( layer._showInterior ),
  _showBorder ( layer._showBorder ),
  _borderColor ( layer._borderColor ),
  _borderWidth ( layer._borderWidth )
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::_registerMembers()
{
  USUL_TRACE_SCOPE;
  SERIALIZE_XML_ADD_MEMBER ( _showInterior );
  SERIALIZE_XML_ADD_MEMBER ( _showBorder );
  SERIALIZE_XML_ADD_MEMBER ( _borderColor );
  SERIALIZE_XML_ADD_MEMBER ( _borderWidth );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PolygonLayer::clone() const
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IUnknown::QueryPtr copy  ( new PolygonLayer( *this ) );
  return copy.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::~PolygonLayer()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::_setGeometryMembers( Geometry* geometry, const pqxx::result::const_iterator& iter )
{
  USUL_TRACE_SCOPE;
  typedef Minerva::Core::Geometry::Polygon Polygon;

  if ( Polygon* polygon = dynamic_cast<Polygon*> ( geometry ) )
  {
    polygon->showBorder( this->showBorder() );
    polygon->showInterior ( this->showInterior() );
    polygon->borderColor ( this->borderColor() );
    polygon->width ( this->borderWidth() );

    // Set tessellate to true for backwards compatabilty.  Need to make this an option.
    polygon->tessellate ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show border flag.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::showBorder( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _showBorder = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show border flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonLayer::showBorder() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _showBorder;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the border color.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::borderColor( const Color& color )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _borderColor = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the border color.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f PolygonLayer::borderColor() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _borderColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show interior flag.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::showInterior( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _showInterior = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show interior flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonLayer::showInterior() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _showInterior;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the border width flag.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::borderWidth( float width )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _borderWidth = width;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the border width flag.
//
///////////////////////////////////////////////////////////////////////////////

float PolygonLayer::borderWidth() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _borderWidth;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PolygonLayer::queryInterface( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  
  switch ( iid )
  {
  case Minerva::Interfaces::IPolygonLayer::IID:
    return static_cast < Minerva::Interfaces::IPolygonLayer* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}
