
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/Factory.h"

#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/Model.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Data/Polygon.h"

#include "XmlTree/Node.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Strings/Split.h"

#include <vector>

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef XmlTree::Node::Children    Children;
typedef Usul::Convert::Type<std::string,double> ToDouble;
typedef Minerva::Core::Data::Geometry Geometry;
typedef Usul::Math::Vec3d                          Vertex;
typedef std::vector < Vertex >                     Vertices;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize static member.
//
///////////////////////////////////////////////////////////////////////////////

Factory* Factory::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance.
//
///////////////////////////////////////////////////////////////////////////////

Factory& Factory::instance()
{
  if ( 0x0 == _instance )
    _instance = new Factory();
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::~Factory()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//  Parse coordinates.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper 
{
  inline void parseCoordinates ( const XmlTree::Node& node, Vertices& vertices )
  {
    std::istringstream in ( node.value() );
    std::string vertex;
    
    while ( in >> vertex )
    {
      Vertex v;
      
      std::vector<std::string> strings;
      Usul::Strings::split ( vertex, ",", false, strings );
      
      // Convert vertex.
      for ( unsigned int i = 0; i < strings.size(); ++i )
        v[i] = ToDouble::convert ( strings[i] );
      
      vertices.push_back ( v );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse orientation.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
 inline osg::Vec3d parseOrientation ( const XmlTree::Node& node )
  {
    Children heading ( node.find ( "heading", false ) );
    Children tilt    ( node.find ( "tilt", false ) );
    Children roll    ( node.find ( "roll", false ) );
    
    const double h ( heading.empty() ? 0.0 : ToDouble::convert ( heading.front()->value() ) );
    const double t ( tilt.empty()    ? 0.0 : ToDouble::convert ( tilt.front()->value() ) );
    const double r ( roll.empty()    ? 0.0 : ToDouble::convert ( roll.front()->value() ) );
    
    return osg::Vec3d ( h, t, r );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a Vec3.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  osg::Vec3 buildVec3 ( const XmlTree::Node& node, osg::Vec3::value_type defaultValue )
  {
    osg::Vec3 v;
    
    Children children ( node.children() );
    
    v[0] = children.size() >= 1 ? ToDouble::convert ( children[0]->value() ) : defaultValue;
    v[1] = children.size() >= 2 ? ToDouble::convert ( children[1]->value() ) : defaultValue;
    v[2] = children.size() >= 3 ? ToDouble::convert ( children[2]->value() ) : defaultValue;
    
    return v;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the altitude mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline Geometry::AltitudeMode parseAltitudeMode ( const XmlTree::Node& node )
  {
    Geometry::AltitudeMode mode ( Geometry::CLAMP_TO_GROUND );
    if ( "relativeToGround" == node.value() )
      mode = Geometry::RELATIVE_TO_GROUND;
    if ( "absolute" == node.value() )
      mode = Geometry::ABSOLUTE_MODE;
    
    return mode;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a point.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Point* Factory::createPoint ( const XmlTree::Node& node ) const
{
  Minerva::Core::Data::Point::RefPtr point ( new Minerva::Core::Data::Point );
  point->autotransform ( true );
  point->size ( 5 );
  point->primitiveId ( 2 );
  point->color ( Usul::Math::Vec4f ( 1.0, 0.0, 0.0, 1.0 ) );
  
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "altitudeMode" == name )
      point->altitudeMode ( Helper::parseAltitudeMode ( *node ) );
    else if ( "coordinates" == name )
    {
      Vertices vertices;
      Helper::parseCoordinates( *node, vertices );
      if ( false == vertices.empty() )
      {
        point->point ( vertices.front() );
      }
    }
    else if ( "extrude" == name )
    {
      bool extrude ( "1" == node->value() );
      point->extrude ( extrude );
    }
  }
  
  return point.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Line* Factory::createLine ( const XmlTree::Node& node ) const
{
  Minerva::Core::Data::Line::RefPtr line ( new Minerva::Core::Data::Line );

  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "altitudeMode" == name )
      line->altitudeMode ( Helper::parseAltitudeMode ( *node ) );
    else if ( "coordinates" == name )
    {
      Vertices vertices;
      Helper::parseCoordinates( *node, vertices );
      if ( false == vertices.empty() )
      {
        Minerva::Core::Data::Line::RefPtr data ( new Minerva::Core::Data::Line );
        line->line ( vertices );
      }
    }
  }
  
  return line.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a polygon.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Polygon* Factory::createPolygon ( const XmlTree::Node& node ) const
{
  // Make the data object.
  Minerva::Core::Data::Polygon::RefPtr polygon ( new Minerva::Core::Data::Polygon );
  
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "altitudeMode" == name )
      polygon->altitudeMode ( Helper::parseAltitudeMode ( *node ) );
    else if ( "outerBoundaryIs" == name )
    {
      XmlTree::Node::RefPtr linearRing ( node->children().front() );
      XmlTree::Node::RefPtr coordinates ( linearRing.valid() ? linearRing->children().front() : 0x0 );
      if ( coordinates.valid() )
      {
        Vertices vertices;
        Helper::parseCoordinates( *coordinates, vertices );
        polygon->outerBoundary ( vertices );
      }
    }
    else if ( "innerBoundaryIs" == name )
    {
      XmlTree::Node::RefPtr linearRing ( node->children().front() );
      XmlTree::Node::RefPtr coordinates ( linearRing.valid() ? linearRing->children().front() : 0x0 );
      if ( coordinates.valid() )
      {
        Vertices vertices;
        Helper::parseCoordinates( *coordinates, vertices );
        polygon->addInnerBoundary ( vertices );
      }
    }
  }
  
  return polygon.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a model.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Model* Factory::createModel ( const XmlTree::Node& node ) const
{
  Minerva::Core::Data::Model::RefPtr model ( new Minerva::Core::Data::Model );
  
  Children children ( node.children() );
  
  osg::Vec3 location, orientation, scale ( 1.0, 1.0, 1.0 );
  
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "Location" == name )
      location = Helper::buildVec3 ( *node, 0.0 );
    else if ( "Orientation" == name )
      orientation = Helper::parseOrientation ( *node );
    else if ( "Scale" == name )
      scale = Helper::buildVec3 ( *node, 1.0 );
    else if ( "altitudeMode" == name )
      model->altitudeMode ( Helper::parseAltitudeMode ( *node ) );
  }
  
  model->location ( location );
  model->orientation ( orientation[0], orientation[1], orientation[2] );
  model->scale ( scale );
  
  return model.release();
}
