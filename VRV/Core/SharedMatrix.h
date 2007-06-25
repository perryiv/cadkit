
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

#include "VRV/Export.h"

#include "osg/Matrixd"

#include "vpr/IO/SerializableObject.h"

namespace vpr { class ObjectReader; class ObjectWriter; }

namespace VRV {
namespace Core {

class VRV_EXPORT SharedMatrix : public vpr::SerializableObject
{
public:
  typedef vpr::SerializableObject BaseClass;

  SharedMatrix();
  virtual ~SharedMatrix();

  /// Read/Write the object.
  virtual vpr::ReturnStatus readObject ( vpr::ObjectReader *reader );
  virtual vpr::ReturnStatus writeObject ( vpr::ObjectWriter *writer );

  /// Get/Set the matrix.
  const osg::Matrixd&       matrix() const            { return _matrix; }
  void                      matrix( osg::Matrixd& m ) { _matrix = m ; }

private:
  osg::Matrixd _matrix;
};

}
}

#endif // __VRV_CORE_SHARED_MATRIX_H__
