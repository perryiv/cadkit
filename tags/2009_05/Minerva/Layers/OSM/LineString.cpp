
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convert Way into LineString for drawing.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/LineString.h"

#include "Minerva/Core/Data/Line.h"

using namespace Minerva::Layers::OSM;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineString::LineString ( Way::IdType id, const Date& timestamp, const Tags& tags, const Vertices& vertices, const NodeIds& ids ) : 
  BaseClass ( id, timestamp, tags ),
  _id ( id ),
  _vertices ( vertices ),
  _ids ( ids )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LineString::~LineString()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create from a way.
//
///////////////////////////////////////////////////////////////////////////////

LineString* LineString::create ( Way::RefPtr way )
{
  Way::IdType id ( 0 );
  Vertices vertices;
  NodeIds ids;
  Date timestamp;
  Tags tags;

  if ( way.valid() )
  {
    id = way->id();
    timestamp = way->timestamp();
    tags = way->tags();

    Nodes nodes ( way->nodes() );

    vertices.reserve ( nodes.size() );
    ids.reserve ( nodes.size() );

    for ( Nodes::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter )
    {
      Node::RefPtr node ( *iter );
      if ( node.valid() )
      {
        vertices.push_back ( node->location() );
        ids.push_back ( node->id() );
      }
    }
  }

  return new LineString ( id, timestamp, tags, vertices, ids );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create from data.
//
///////////////////////////////////////////////////////////////////////////////

LineString* LineString::create ( Way::IdType id, const Date& timestamp, const Tags& tags, const Vertices& vertices, const NodeIds& ids )
{
  return new LineString ( id, timestamp, tags, vertices, ids );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the geometry.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::Line* LineString::buildGeometry ( Minerva::Core::Data::LineStyle* style, unsigned int stride ) const
{
  // Make a line.
  Minerva::Core::Data::Line::RefPtr line ( new Minerva::Core::Data::Line );
  Minerva::Core::Data::Line::Vertices vertices;
  vertices.reserve ( _vertices.size() );

  for ( unsigned int i = 0; i < _vertices.size(); i += stride )
  {
    Node::Location location ( _vertices.at ( i ) );
    vertices.push_back ( Usul::Math::Vec3d ( location[0], location[1], 0.0 ) );
  }

  if ( ( 1 != stride ) && ( false == _vertices.empty() ) && ( 0 != ( _vertices.size() % stride ) ) )
  {
    Node::Location location ( _vertices.back() );
    vertices.push_back ( Usul::Math::Vec3d ( location[0], location[1], 0.0 ) );
  }

  line->line ( vertices );
  line->lineStyle ( style );

  line->useShader ( true );

  return line.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the ids.
//
///////////////////////////////////////////////////////////////////////////////

const LineString::NodeIds& LineString::ids() const
{
  return _ids;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertices.
//
///////////////////////////////////////////////////////////////////////////////

const LineString::Vertices& LineString::vertices() const
{
  return _vertices;
}
