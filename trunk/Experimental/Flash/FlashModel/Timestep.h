
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

#include "osg/BoundingBox"

#include <vector>

namespace osg { class Node; }

class H5File;

class Timestep : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( Timestep );
  
  Timestep ( const std::string& filename );
  
  void init();
  
  osg::Node* buildScene();
  
protected:
  
  virtual ~Timestep();
  
private:
  
  void     _initBoudingBoxes ( H5File& file );
  void     _initLeafFlags    ( H5File& file );
  void     _initLevels       ( H5File& file );
  
  typedef std::vector<osg::BoundingBox> BoundingBoxes;
  typedef std::vector<bool> LeafFlags;
  typedef std::vector<int> Levels;
  
  std::string _filename;
  BoundingBoxes _boundingBoxes;
  LeafFlags _leafFlags;
  Levels _levels;
};

#endif // __FLASH_MODEL_TIMESTEP_H__
