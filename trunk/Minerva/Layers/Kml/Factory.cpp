
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/Factory.h"
#include "Minerva/Layers/Kml/NetworkLink.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Feature.h"
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/LookAt.h"
#include "Minerva/Core/Data/Model.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Data/Polygon.h"
#include "Minerva/Core/Data/TimeSpan.h"
#include "Minerva/Core/Data/Style.h"
#include "Minerva/Core/Data/IconStyle.h"
#include "Minerva/Core/Data/PolyStyle.h"
#include "Minerva/Core/Data/LineStyle.h"
#include "Minerva/Core/Data/Link.h"

#include "XmlTree/Node.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Split.h"

#include "boost/algorithm/string/trim.hpp"

#include <algorithm>

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef XmlTree::Node::Children                 Children;
typedef Usul::Convert::Type<std::string,float>  ToFloat;
typedef Usul::Convert::Type<std::string,double> ToDouble;
typedef Minerva::Core::Data::Geometry           Geometry;


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
//  Set data members in object base class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setObjectDataMembers ( Minerva::Core::Data::Object *object, const XmlTree::Node& node )
  {
    if ( 0x0 != object )
    {
      typedef XmlTree::Node::Attributes Attributes;
      
      // Get the attributes.
      const Attributes& attributes ( node.attributes() );
      
      // Set the id.
      {
        Attributes::const_iterator iter ( attributes.find ( "id" ) );
        object->objectId ( iter != attributes.end() ? iter->second : "" );
      }
      
      // Set the target id.
      {
        Attributes::const_iterator iter ( attributes.find ( "targetId" ) );
        object->targetId ( iter != attributes.end() ? iter->second : "" );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set data members in feature class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setFeatureDataMembers ( Minerva::Core::Data::Feature& feature, const XmlTree::Node& node )
  {
    std::string name ( node.name() );
    
    if ( "name" == name )
    {
      feature.name ( node.value() );
    }
    else if ( "visibility" == name )
    {
      bool visible ( "0" != node.value() );
      feature.visibility ( visible );
    }
    else if ( "LookAt" == name )
    {
      feature.lookAt ( Factory::instance().createLookAt ( node ) );
    }
    else if ( "styleUrl" == name )
    {
      feature.styleUrl ( node.value() );
    }
    else if ( "TimeSpan" == name )
    {
      feature.timePrimitive ( Factory::instance().createTimeSpan ( node ) );
    }
    else if ( "description" == name )
    {
      feature.description ( node.value() );
    }
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set data members in ColorStyle class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setColorStyleDataMembers ( Minerva::Core::Data::ColorStyle *colorStyle, const Children& children )
  {
    typedef Minerva::Core::Data::ColorStyle ColorStyle;
    
    if ( 0x0 != colorStyle )
    {
      for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
      {
        XmlTree::Node::RefPtr node ( (*iter).get() );
        std::string name ( node->name() );
        
        if ( "color" == name )
        {
          ColorStyle::Color color;
          
          const unsigned long c ( ::strtoul ( node->value().c_str(), 0x0, 16 ) );
          
          color[3] = static_cast<float> ( ( ( c & 0xff000000 ) >> 24 ) / 255.0 );
          color[2] = static_cast<float> ( ( ( c & 0x00ff0000 ) >> 16 ) / 255.0 );
          color[1] = static_cast<float> ( ( ( c & 0x0000ff00 ) >>  8 ) / 255.0 );
          color[0] = static_cast<float> ( ( ( c & 0x000000ff )       ) / 255.0 );
          
          colorStyle->color ( color );
        }
        else if ( "colorMode" == name )
        {
          colorStyle->mode ( "random" == node->value() ? ColorStyle::RANDOM : ColorStyle::NORMAL );
        }
      }
      
      // Make a random color if we are suppose to.
      if ( ColorStyle::RANDOM == colorStyle->mode() )
      {
        colorStyle->color ( ColorStyle::makeRandomColor ( colorStyle->color() ) );
      }
    }
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to skip over white space.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void skipWhiteSpace ( char*& pointer )
  {
    while ( ::isspace ( *pointer ) )
    {
      ++pointer;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Parse a vec3.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  bool parseVec3 ( const char* start, char** next, Factory::Vertex& v )
  {
    if ( 0x0 == start )
      return false;

    char *current ( const_cast<char*> ( start ) );

    // strtod will skip white space and stop at a character that is doesn't reconize as part of a number.
    // See: http://msdn.microsoft.com/en-us/library/53b7b72e.aspx
    const double longitude ( ::strtod ( current, &current ) );
    v[0] = longitude;

    // Eat white space up to the next comma
    while ( ::isspace ( *current ) || ',' != *current )
    {
      // Check for bad string.
      if ( *current == '\0' )
      {
        *next = current;
        return false;
      }

      ++current;
    }

    const double latitude ( ::strtod ( current + 1, &current ) );
    v[1] = latitude;

    // Eat all white space.
    Helper::skipWhiteSpace ( current );

    const double altitude ( ',' == *current ? ::strtod ( current + 1, &current ) : 0.0 );
    v[2] = altitude;

    // Eat remaining white space.
    Helper::skipWhiteSpace ( current );

    *next = current;
    return true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::parseCoordinates ( const XmlTree::Node& node, Vertices& vertices, Extents& extents ) const
{
  // Copy the value and remove any commas.
  std::string value ( node.value() );

  if ( true == value.empty() )
    return;

  const char* characters ( value.c_str() );
  const char* end ( characters + value.size() );
  char* next ( const_cast<char*> ( characters ) );

  while ( next != end )
  {
    Vertex vertex;
    if ( Helper::parseVec3 ( next, &next, vertex ) )
    {
      vertices.push_back ( vertex );
      extents.expand ( Extents::Vertex ( vertex[0], vertex[1] ) );
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
    
    const Children& children ( node.children() );
    
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
  point->primitiveId ( Minerva::Core::Data::Point::NONE );
  point->color ( Usul::Math::Vec4f ( 1.0, 0.0, 0.0, 1.0 ) );
  
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( (*iter).get() );
    std::string name ( node->name() );
    
    if ( "altitudeMode" == name )
      point->altitudeMode ( Helper::parseAltitudeMode ( *node ) );
    else if ( "coordinates" == name )
    {
      Vertices vertices;
      Extents extents;
      this->parseCoordinates ( *node, vertices, extents );
      if ( false == vertices.empty() )
      {
        point->point ( vertices.front() );
        point->extents ( extents );
      }
    }
    else if ( "extrude" == name )
    {
      bool extrude ( "1" == node->value() );
      point->extrude ( extrude );
    }
  }
  
  Helper::setObjectDataMembers ( point.get(), node );
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

  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( (*iter).get() );
    std::string name ( node->name() );
    
    if ( "altitudeMode" == name )
      line->altitudeMode ( Helper::parseAltitudeMode ( *node ) );
    else if ( "coordinates" == name )
    {
      Vertices vertices;
      Extents extents;
      this->parseCoordinates( *node, vertices, extents );
      if ( false == vertices.empty() )
      {
        line->line ( vertices );
        line->extents ( extents );
      }
      else if ( "extrude" == name )
      {
        bool extrude ( "1" == node->value() );
        line->extrude ( extrude );
      }
    }
  }
  
  Helper::setObjectDataMembers ( line.get(), node );
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
  
  Extents extents;
  
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( (*iter).get() );
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
        this->parseCoordinates( *coordinates, vertices, extents );
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
        this->parseCoordinates( *coordinates, vertices, extents );
        polygon->addInnerBoundary ( vertices );
      }
    }
    else if ( "extrude" == name )
    {
      bool extrude ( "1" == node->value() );
      polygon->extrude ( extrude );
    }
  }
  
  polygon->extents ( extents );
  
  Helper::setObjectDataMembers ( polygon.get(), node );
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
  
  osg::Vec3 location, orientation, scale ( 1.0, 1.0, 1.0 );
  
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( (*iter).get() );
    std::string name ( node->name() );
    
    if ( "Location" == name )
      location = Helper::buildVec3 ( *node, 0.0 );
    else if ( "Orientation" == name )
      orientation = Helper::parseOrientation ( *node );
    else if ( "Scale" == name )
      scale = Helper::buildVec3 ( *node, 1.0 );
    else if ( "altitudeMode" == name )
      model->altitudeMode ( Helper::parseAltitudeMode ( *node ) );
    else if ( "Link" == name )
      model->link ( this->createLink ( *node ) );
  }
  
  model->location ( location );
  model->orientation ( orientation[0], orientation[1], orientation[2] );
  model->scale ( scale );
  
  // Set the extents.  It may be better to calcuate the model's footprint and use that as the extents.
  model->extents ( Extents ( Extents::Vertex ( location[0], location[1] ), Extents::Vertex ( location[0], location[1] ) ) );
  
  Helper::setObjectDataMembers ( model.get(), node );
  return model.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a MultiGeometry.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::createMultiGeometry ( const XmlTree::Node& node, PlaceMark& placeMark ) const
{
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    Geometry::RefPtr geometry ( 0x0 );

    XmlTree::Node::RefPtr child ( (*iter).get() );
    const std::string name ( child->name() );

    // Pick which function to redirect to.
    if ( "Model" == name )
    {
      geometry = this->createModel ( *child );
    }
    else if ( "Point" == name )
    {
      geometry = this->createPoint ( *child );

      // The PlaceMark has a point, turn on labels.
      placeMark.showLabel ( true );
    }
    else if ( "Polygon" == name )
    {
      geometry = this->createPolygon ( *child );
    }
    else if ( "LineString" == name )
    {
      geometry = this->createLine ( *child );
    }
    else if ( "LineRing" == name )
    {
      geometry = this->createLine ( *child );
    }

    if ( geometry.valid() )
    {
      placeMark.addGeometry ( geometry );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a LookAt.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::LookAt* Factory::createLookAt ( const XmlTree::Node& node ) const
{
  Minerva::Core::Data::LookAt::RefPtr lookAt ( new Minerva::Core::Data::LookAt );
  
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( (*iter).get() );
    std::string name ( node->name() );
    
    if ( "longitude" == name )
    {
      lookAt->longitude ( ToDouble::convert ( node->value() ) );
    }
    else if ( "latitude" == name )
    {
      lookAt->latitude ( ToDouble::convert ( node->value() ) );
    }
    else if ( "altitude" == name )
    {
      lookAt->altitude ( ToDouble::convert ( node->value() ) );
    }
    else if ( "range" == name )
    {
      lookAt->range ( ToDouble::convert ( node->value() ) );
    }
    else if ( "heading" == name )
    {
      lookAt->heading ( ToDouble::convert ( node->value() ) );
    }
  }

  
  Helper::setObjectDataMembers ( lookAt.get(), node );
  return lookAt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a TimeSpan.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::TimeSpan* Factory::createTimeSpan ( const XmlTree::Node& node ) const
{
  Minerva::Core::Data::TimeSpan::RefPtr span ( new Minerva::Core::Data::TimeSpan );

  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( (*iter).get() );
    std::string name ( node->name() );
    
    if ( "begin" == name )
    {
      span->begin ( Minerva::Core::Animate::Date::createFromKml ( node->value() ) );
    }
    else if ( "end" == name )
    {
      span->end ( Minerva::Core::Animate::Date::createFromKml ( node->value() ) );
    }
  }
  
  Helper::setObjectDataMembers ( span.get(), node );
  return span.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a PlaceMark.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::DataObject* Factory::createPlaceMark ( const XmlTree::Node& node ) const
{
  PlaceMark::RefPtr object ( new PlaceMark );
  object->showLabel ( false );
  object->labelPosition ( PlaceMark::PositionType ( 0.0, 0.0, 1.0 ) );
 
  const Children& children ( node.children() );
  
  Style::RefPtr style ( 0x0 );

  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr child ( (*iter).get() );

    Helper::setFeatureDataMembers ( *object, *child );

    const std::string name ( child->name() );
    if ( "Style" == name )
    {
      style = Factory::instance().createStyle ( *child );
    }
    else if ( "Model" == name )
    {
      object->addGeometry ( this->createModel ( *child ) );
    }
    else if ( "Point" == name )
    {
      object->addGeometry ( this->createPoint ( *child ) );

      // Google Earth only appears to label points.
      object->showLabel ( true );
    }
    else if ( "Polygon" == name )
    {
      object->addGeometry ( this->createPolygon ( *child ) );
    }
    else if ( "LineString" == name )
    {
      object->addGeometry ( this->createLine ( *child ) );
    }
    else if ( "LineRing" == name )
    {
      object->addGeometry ( this->createLine ( *child ) );
    }
    else if ( "MultiGeometry" == name )
    {
      this->createMultiGeometry ( *child, *object );
    }
  }

  // Set the label.
  object->label ( object->name() );

  Extents extents;

  PlaceMark::Geometries geometries ( object->geometries() );
  for ( PlaceMark::Geometries::iterator iter = geometries.begin(); iter != geometries.end(); ++iter )
  {
    Geometry::RefPtr geometry ( *iter );
    if ( geometry.valid() )
    {
      geometry->style ( style );
      extents.expand ( geometry->extents() );
    }
  }

  object->extents ( extents );

  Helper::setObjectDataMembers ( object.get(), node );
  
  // Set the data object members.
  object->labelColor ( Minerva::Core::Data::DataObject::ColorType ( 1.0, 1.0, 1.0, 1.0 ) );
  
  return object.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a Link.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Link* Factory::createLink ( const XmlTree::Node& node ) const
{
  Link::RefPtr link ( new Link );
  
  // Get the children.
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr child ( (*iter).get() );
    std::string name ( child->name() );
    
    if ( "href" == name )
      link->href ( child->value() );
    else if ( "refreshInterval" == name )
      link->refreshInterval ( ToDouble::convert ( child->value() ) );
    else if ( "refreshMode" == name )
    {
      std::string mode ( child->value() );
      if ( "onChange" == mode )
        link->refreshMode ( Link::ON_CHANGE );
      else if ( "onInterval" == mode )
        link->refreshMode ( Link::ON_INTERVAL );
      else if ( "onExpire" == mode )
        link->refreshMode ( Link::ON_EXPIRE );
    }
  }
  
  Helper::setObjectDataMembers ( link.get(), node );
  return link.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a NetworkLink.
//
///////////////////////////////////////////////////////////////////////////////

NetworkLink* Factory::createNetworkLink ( const XmlTree::Node& node ) const
{
  NetworkLink::RefPtr networkLink ( new NetworkLink );
  
  // Loop over the children.
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr child ( (*iter).get() );
    std::string name ( Usul::Strings::lowerCase ( child->name() ) );
    
    if ( "link" == name || "url" == name ) // Url is an older name, but many elements are the same.
    {
      networkLink->link ( Factory::instance().createLink ( *child ) );
    }

    Helper::setFeatureDataMembers ( *networkLink, *child );
  }
  
  Helper::setObjectDataMembers ( networkLink.get(), node );
  return networkLink.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a Style.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::Style* Factory::createStyle ( const XmlTree::Node& node ) const
{
  Style::RefPtr style ( new Style );
  
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr child ( (*iter).get() );
    std::string name ( child->name() );
    
    if ( "PolyStyle" == name )
    {
			style->polystyle ( Factory::instance().createPolyStyle ( *child ) );
    }
    else if ( "LineStyle" == name )
    {
      style->linestyle ( Factory::instance().createLineStyle ( *child ) );
    }
    else if ( "IconStyle" == name )
    {
      style->iconstyle ( Factory::instance().createIconStyle ( *child ) );
    }
  }
  
  Helper::setObjectDataMembers ( style.get(), node );
  return style.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a LineStyle.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::LineStyle* Factory::createLineStyle ( const XmlTree::Node& node ) const
{
  LineStyle::RefPtr lineStyle ( new LineStyle );
  
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( (*iter).get() );
    std::string name ( node->name() );
    
    if ( "width" == name )
    {
      lineStyle->width ( ToFloat::convert ( node->value() ) );
    }
  }
  
  Helper::setColorStyleDataMembers ( lineStyle.get(), children );
  Helper::setObjectDataMembers ( lineStyle.get(), node );
  return lineStyle.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a PolyStyle.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::PolyStyle* Factory::createPolyStyle ( const XmlTree::Node& node ) const
{
  PolyStyle::RefPtr polyStyle ( new PolyStyle );
  
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( (*iter).get() );
    std::string name ( node->name() );
    
    if ( "fill" == name )
    {
      polyStyle->fill ( "1" == node->value() );
    }
    else if ( "outline" == name )
    {
      polyStyle->outline ( "1" == node->value() );
    }
  }
  
  Helper::setColorStyleDataMembers ( polyStyle.get(), children );
  Helper::setObjectDataMembers ( polyStyle.get(), node );
  return polyStyle.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a IconStyle.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::IconStyle* Factory::createIconStyle ( const XmlTree::Node& node ) const
{
  IconStyle::RefPtr iconStyle ( new IconStyle );
  
  const Children& children ( node.children() );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( (*iter).get() );
    std::string name ( node->name() );
    
    if ( "scale" == name )
    {
      iconStyle->scale ( ToFloat::convert ( node->value() ) );
    }
    else if ( "heading" == name )
    {
      iconStyle->heading ( ToFloat::convert ( node->value() ) );
    }
    else if ( "Link" == name )
    {
      Link::RefPtr link ( Factory::instance().createLink ( *node ) );
      iconStyle->href ( link->href() );
    }
  }
  
  Helper::setColorStyleDataMembers ( iconStyle.get(), children );
  Helper::setObjectDataMembers ( iconStyle.get(), node );
  return iconStyle.release();
}
