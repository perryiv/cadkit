
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODFLOW_ATTRIBUTES_HEAD_SURFACE_CLASS_H__
#define __MODFLOW_ATTRIBUTES_HEAD_SURFACE_CLASS_H__

#include "ModflowModel/Attributes/HeadLevels.h"

namespace Modflow {
namespace Attributes {

  
class HeadSurface : public Modflow::Attributes::HeadLevels
{
public:
  typedef Modflow::Attributes::HeadLevels BaseClass;
  
  // Type information.
  USUL_DECLARE_TYPE_ID ( HeadSurface );
  
  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HeadSurface );
  
  // Constructor.
  HeadSurface ( const std::string &name, Modflow::Model::Layer *layer, unsigned int numSubdivide, unsigned int numSmooth, const RegistryPath & );
  
protected:
  // Use reference counting.
  virtual ~HeadSurface();

  virtual osg::Node *        _buildTimeStep ( unsigned int timeStep, Modflow::ModflowDocument *document, Modflow::Model::Layer *, bool hasNoData, long noData, const Vec3d &offset, double scale, std::ostream *out = 0x0 ) const;
  
private:
  unsigned int _numSubdivide;
  unsigned int _numSmooth;
};


}
}

#endif // __MODFLOW_ATTRIBUTES_HEAD_SURFACE_CLASS_H__
