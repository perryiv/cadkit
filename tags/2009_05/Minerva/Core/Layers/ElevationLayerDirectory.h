
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Layer that treats whole directory as a single elevation layer.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ELEVATION_LAYER_DIRECTORY_H__
#define __MINERVA_CORE_ELEVATION_LAYER_DIRECTORY_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/ElevationGroup.h"

namespace osg { class Image; }

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT ElevationLayerDirectory : public ElevationGroup
{
public:
  
  typedef ElevationGroup BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( ElevationLayerDirectory );
  
  ElevationLayerDirectory();
  
  /// Clone.
  virtual IUnknown*     clone() const;
  
  /// Deserialize.
  virtual void          deserialize ( const XmlTree::Node& node );
  
  /// Set/get the directory.
  void                  directory ( const std::string& );
  std::string           directory() const;
  
  /// Read the directory.
  void                  readDirectory();
  
  /// Serialize.
  virtual void          serialize ( XmlTree::Node &parent ) const;
  
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );
  
  /// Get the raster data as elevation data.
  virtual IElevationData::RefPtr elevationData ( 
    double minLon,
    double minLat,
    double maxLon,
    double maxLat,
    unsigned int width,
    unsigned int height,
    unsigned int level,
    Usul::Jobs::Job* job,
    Usul::Interfaces::IUnknown* caller );

protected:
  
  virtual ~ElevationLayerDirectory();
  
  ElevationLayerDirectory ( const ElevationLayerDirectory& );
  
  // Get the number of children (ITreeNode).
  virtual unsigned int            getNumChildNodes() const;
  
  // Get the child node (ITreeNode).
  virtual ITreeNode::RefPtr       getChildNode ( unsigned int which );
  
private:

  // No assignment.
  ElevationLayerDirectory& operator= ( const ElevationLayerDirectory& );
  
  std::string _directory;
  
  SERIALIZE_XML_CLASS_NAME ( ElevationLayerDirectory );
};

} // namespace Layers
} // namespace Core
} // namespace Minerva

#endif // __MINERVA_CORE_ELEVATION_GROUP_H__
