
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Layer to display names from GeoNames (www.geonames.org) web service.
//  This layer is a proof of concept for tiling vector data.
//
//  TODO:
//    -Handle removing and hiding the layer (The current architechure doesn't support this well).
//    -Vary the height and size of text based on the current split level.
//    -There appears to be a bug when the extents covers North America.  Cities do not show up, even though they are in the GeoNames database.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_GEO_NAMES_LAYER_H__
#define __MINERVA_LAYERS_GEO_NAMES_LAYER_H__

#include "Minerva/Layers/GeoNames/Export.h"
#include "Minerva/Layers/GeoNames/City.h"

#include "Minerva/Core/Data/Container.h"

#include "osg/Node"

#include <vector>
#include <map>

namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace GeoNames {
  
class MINERVA_GEO_NAMES_EXPORT CityLayer : public Minerva::Core::Data::Feature,
                                           public Minerva::Interfaces::ITilesChangedListener
{
public:
  
  /// Typedefs.
  typedef Minerva::Core::Data::Feature    BaseClass;
  typedef std::vector<City>               Cities;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( CityLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  CityLayer();

  // Get all the cities in the extents up to the maximum number allowed.
  Cities                      citiesGet ( const Extents& extents, unsigned int level, unsigned int maximumItems ) const;
  
  // Deserialize.
  virtual void                deserialize( const XmlTree::Node &node );

  /// Serialize.
  virtual void                serialize ( XmlTree::Node &parent ) const;

  // Notification of tiles added or removed.
  virtual void                tileAddNotify ( Tile::RefPtr child, Tile::RefPtr parent );
  virtual void                tileRemovedNotify ( Tile::RefPtr child, Tile::RefPtr parent );

protected:

  virtual ~CityLayer();

  // Add the node to the tile.
  void                        _addNode ( Tile::RefPtr tile, osg::Node* node );

  // Build the scene.
  osg::Node*                  _buildScene ( Tile::RefPtr tile, const Cities& cities );

  // Get all the cities in the extents up to the maximum number allowed.
  void                        _citiesGet ( Cities& cities, const std::string& filename, const Extents& extents, unsigned int level, unsigned int maximumItems ) const;

  // Make the cache directory.
  std::string                 _makeCacheDirectory ( const Extents& extents, unsigned int level ) const;

  // Remove the node that was added to the tile.
  void                        _removeNode ( Tile::RefPtr tile );

private:

  typedef std::map< Tile::RefPtr, osg::ref_ptr<osg::Node> > NodeMap;

  NodeMap _nodeMap;
  
  SERIALIZE_XML_CLASS_NAME ( CityLayer );
};
  
}
}
}


#endif // __MINERVA_LAYERS_GEO_NAMES_LAYER_H__