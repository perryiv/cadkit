
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

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_LINE_STRING_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_LINE_STRING_H__

#include "Minerva/Layers/OSM/Common.h"

#include <vector>

namespace Minerva { namespace Core { namespace Data { class Line; class LineStyle; } } }

namespace Minerva {
namespace Layers {
namespace OSM {

class MINERVA_OSM_EXPORT LineString : public Object
{
public:

  typedef Object BaseClass;
  typedef std::vector<Usul::Math::Vec2d> Vertices;
  typedef std::vector<Node::IdType> NodeIds;

  USUL_DECLARE_REF_POINTERS ( LineString );

  static LineString* create ( Way::RefPtr way );
  static LineString* create ( IdType id, const Date& timestamp, const Tags& tags, const Vertices& vertices, const NodeIds& ids );
  
  /// Build the geometry.
  Minerva::Core::Data::Line* buildGeometry ( Minerva::Core::Data::LineStyle* style, unsigned int stride ) const;

  const NodeIds& ids() const;
  const Vertices& vertices() const;

protected:

  virtual ~LineString();

private:

  LineString();
  LineString ( IdType id, const Date& timestamp, const Tags& tags, const Vertices& vertices, const NodeIds& ids );
  LineString ( const LineString& );
  LineString& operator= ( const LineString& );

  IdType _id;
  Vertices _vertices;
  NodeIds _ids;
  
};


}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_LINE_STRING_H__
