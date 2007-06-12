
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for sharing an osg::Matrix across all nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_CORE_SHARED_MATRIX_H__
#define __VRV_CORE_SHARED_MATRIX_H__

#include "VRV/Core/Export.h"

#include "vpr/IO/SerializableObject.h"

namespace vpr { class ObjectReader; class ObjectWriter; }

namespace VRV {
namespace Core {

class VRV_EXPORT SharedMatrix : public vpr::SerializableObject
{
public:
  typedef vpr::SerializableObject BaseClass;

  SharedMatrix();

  virtual vpr::ReturnStatus readObject ( vpr::ObjectReader *reader );
  virtual vpr::ReturnStatus writeObject ( vpr::ObjectWriter *writer );

private:
  osg::Matrixd _matrix;
};

}
}

#endif // __VRV_CORE_SHARED_MATRIX_H__
