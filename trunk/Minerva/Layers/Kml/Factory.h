
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
    }
  }
  
namespace Layers {
namespace Kml {
      
class Factory
{
public:
  typedef Minerva::Core::Data::Point   Point;
  typedef Minerva::Core::Data::Line    Line;
  typedef Minerva::Core::Data::Polygon Polygon;
  typedef Minerva::Core::Data::Model   Model;
  
  static Factory& instance();
  
  Point*    createPoint ( const XmlTree::Node& ) const;
  Line*     createLine ( const XmlTree::Node& ) const;
  Polygon*  createPolygon ( const XmlTree::Node& ) const;
  Model*    createModel ( const XmlTree::Node& ) const;
  
private:
  Factory();
  ~Factory();
  
  static Factory* _instance;
};
      
}
}
}

#endif // __MINERVA_LAYERS_KML_FACTORY_H__
