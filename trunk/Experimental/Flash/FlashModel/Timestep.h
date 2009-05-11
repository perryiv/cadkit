
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FLASH_MODEL_TIMESTEP_H__
#define __FLASH_MODEL_TIMESTEP_H__

#include "Usul/Base/Object.h"

#include "OsgTools/Configure/OSG.h"

#include "osg/BoundingBox"
#include "osg/Vec4"

#include "boost/multi_array.hpp"

#include <vector>

namespace osg { class Node; class Image; }

class H5File;

class Timestep : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( Timestep );
  
  Timestep ( const std::string& filename );
  
  /// Get the bounding box.
  osg::BoundingBox  boundingBox ( unsigned int i ) const;
  
  /// Build functions.
  osg::Node*     buildBoundingBox ( const osg::BoundingBox& bb, const osg::Vec4f& color ) const;
  osg::Node*     buildPoints      ( const osg::BoundingBox& bb, unsigned int i ) const;
  osg::Image*    buildVolume      ( unsigned int i, double minimum, double maximum ) const;
  
  /// Initialize.
  void init();
  
  /// Is the node a leaf?
  bool isLeaf( unsigned int i ) const;
  
  /// Get the level of the node.
  int level ( unsigned int i ) const;
  
  /// Load the data with the given name.
  void loadData ( const std::string& name );
  
  /// Get the min and max values.
  double minimum() const;
  double maximum() const;
  
  /// Get the number of nodes.
  unsigned int numNodes() const;
  
protected:
  
  virtual ~Timestep();
  
private:
  
  void     _initBoundingBoxes ( H5File& file );
  void     _initLeafFlags    ( H5File& file );
  void     _initLevels       ( H5File& file );
  void     _initHierarchy    ( H5File& file );
  
  typedef std::vector<osg::BoundingBox> BoundingBoxes;
  typedef std::vector<bool> LeafFlags;
  typedef std::vector<int> Levels;
  typedef boost::multi_array<double, 4> DataArray;
  typedef boost::multi_array<int, 2> HierarchyArray;
  
  std::string _filename;
  BoundingBoxes _boundingBoxes;
  LeafFlags _leafFlags;
  Levels _levels;
  DataArray _data;
  double _minimum;
  double _maximum;
  HierarchyArray _hierarchy;
};

#endif // __FLASH_MODEL_TIMESTEP_H__
