
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Geometry/Line.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IProjectCoordinates.h"

using namespace Minerva::Core::Geometry;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( Line, Line::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::Line() : BaseClass(),
_line (),
_latLongPoints ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::~Line()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to lat long points.
//
///////////////////////////////////////////////////////////////////////////////

void Line::_convertToLatLong ( const Vertices& vertices, Vertices& latLongPoints )
{
  Usul::Interfaces::IProjectCoordinates::QueryPtr project ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IProjectCoordinates::IID ) );

  if ( project.valid() )
  {
    latLongPoints.reserve ( vertices.size() );
    for( Vertices::const_iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
    {
      Usul::Math::Vec3d point;
      project->projectToSpherical( *iter, this->srid(), point );     
      latLongPoints.push_back( point );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the lat long points.
//
///////////////////////////////////////////////////////////////////////////////

void Line::_buildLatLongPoints()
{
  _latLongPoints.clear ();
  this->_convertToLatLong( _line, _latLongPoints );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the line data.
//
///////////////////////////////////////////////////////////////////////////////

const Line::Vertices& Line::lineData ()
{
  if( _latLongPoints.empty() )
  {
    this->_convertToLatLong ( _line, _latLongPoints );
  }

  return _latLongPoints;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Line::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::ILineData::IID:
    return static_cast < Usul::Interfaces::ILineData* > ( this );
  default:
    return BaseClass::queryInterface( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the line data.
//
///////////////////////////////////////////////////////////////////////////////

void Line::line( const Vertices& data )
{
  _line = data;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the line data.
//
///////////////////////////////////////////////////////////////////////////////

const Line::Vertices& Line::line() const
{
  return _line;
}
