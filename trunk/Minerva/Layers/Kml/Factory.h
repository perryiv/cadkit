
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_FACTORY_H__
#define __MINERVA_LAYERS_KML_FACTORY_H__

namespace XmlTree { class Node; }

namespace Minerva {
  
  namespace Core {
    namespace Data {
      class Point;
      class Line;
      class Polygon;
      class Model;
      class TimeSpan;
      class LookAt;
      class DataObject;
      class Style;
      class LineStyle;
      class PolyStyle;
      class IconStyle;
    }
  }
  
namespace Layers {
namespace Kml {
  
  class NetworkLink;
  class Link;
  
class Factory
{
public:
  typedef Minerva::Core::Data::Point      Point;
  typedef Minerva::Core::Data::Line       Line;
  typedef Minerva::Core::Data::Polygon    Polygon;
  typedef Minerva::Core::Data::Model      Model;
  typedef Minerva::Core::Data::TimeSpan   TimeSpan;
  typedef Minerva::Core::Data::LookAt     LookAt;
  typedef Minerva::Core::Data::DataObject PlaceMark;
  typedef Minerva::Core::Data::Style      Style;
  typedef Minerva::Core::Data::IconStyle  IconStyle;
  typedef Minerva::Core::Data::PolyStyle  PolyStyle;
  typedef Minerva::Core::Data::LineStyle  LineStyle;
  
  static Factory& instance();
  
  Point*        createPoint ( const XmlTree::Node& ) const;
  Line*         createLine ( const XmlTree::Node& ) const;
  Polygon*      createPolygon ( const XmlTree::Node& ) const;
  Model*        createModel ( const XmlTree::Node& ) const;
  
  LookAt*       createLookAt ( const XmlTree::Node& ) const;
  
  TimeSpan*     createTimeSpan ( const XmlTree::Node& ) const;
  
  PlaceMark*    createPlaceMark ( const XmlTree::Node& ) const;
  
  Link*         createLink ( const XmlTree::Node& ) const;
  NetworkLink*  createNetworkLink ( const XmlTree::Node& ) const;
  
  Style*        createStyle ( const XmlTree::Node& ) const;
  LineStyle*    createLineStyle ( const XmlTree::Node& ) const;
  PolyStyle*    createPolyStyle ( const XmlTree::Node& ) const;
  IconStyle*    createIconStyle ( const XmlTree::Node& ) const;
  
private:
  
  Factory();
  ~Factory();
  
  static Factory* _instance;
};
      
}
}
}

#endif // __MINERVA_LAYERS_KML_FACTORY_H__
