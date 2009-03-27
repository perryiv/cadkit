
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/PostGIS/LineLayer.h"

#include "Minerva/Core/Data/Line.h"

#include "OsgTools/Convert.h"

#include "Usul/Factory/RegisterCreator.h"

using namespace Minerva::Layers::PostGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( LineLayer, LineLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( LineLayer );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayer::LineLayer() : BaseClass(),
  _lineWidth ( 1.0f )
{
  // Don't set the name!  
  // This will cause a crash because the object will be referenced and dereferenced in the function, cause the object to be deleted.
  //this->name( "LineLayer" );
  
  this->_addMember ( "lineWidth", _lineWidth );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayer::LineLayer ( const LineLayer& layer ) : BaseClass ( layer ),
  _lineWidth ( layer._lineWidth )
{
  this->_addMember ( "lineWidth", _lineWidth );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* LineLayer::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr copy ( new LineLayer( *this ) );
  return copy.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayer::~LineLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the line width.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::lineWidth( float width )
{
  Guard guard( this->mutex() );
  _lineWidth = width;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line width.
//
///////////////////////////////////////////////////////////////////////////////

float LineLayer::lineWidth() const
{
  Guard guard( this->mutex() );
  return _lineWidth;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::_setGeometryMembers( Geometry* geometry, const Minerva::DataSources::Result &result )
{
  typedef Minerva::Core::Data::Line Line;
  typedef Minerva::Core::Data::LineStyle LineStyle;

  if ( Line* line = dynamic_cast<Line*> ( geometry ) )
  {
    LineStyle::RefPtr lineStyle ( new LineStyle );
    lineStyle->color ( Usul::Convert::Type<osg::Vec4f, Usul::Math::Vec4f>::convert ( this->_color ( result ) ) );
    lineStyle->width ( this->lineWidth() );
    
    line->lineStyle ( lineStyle.get() );

    // Set tessellate to true for backwards compatabilty.  Need to make this an option.
    line->tessellate ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* LineLayer::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Minerva::Interfaces::ILineLayer::IID:
    return static_cast < Minerva::Interfaces::ILineLayer* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}
