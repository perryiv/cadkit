
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

#if __VPR_version < 1001005
#define RETURN_TYPE vpr::ReturnStatus
#else
#define RETURN_TYPE void
#endif

namespace VRV {
namespace Core {

class VRV_EXPORT SharedMatrix : public vpr::SerializableObject
{
public:
  typedef vpr::SerializableObject BaseClass;

  SharedMatrix();
  virtual ~SharedMatrix();

  /// Read/Write the object.
  virtual RETURN_TYPE readObject ( vpr::ObjectReader *reader );
  virtual RETURN_TYPE writeObject ( vpr::ObjectWriter *writer );

  /// Get/Set the matrix.
  const osg::Matrixd&       matrix() const            { return _matrix; }
  void                      matrix( const osg::Matrixd& m ) { _matrix = m ; }

private:
  osg::Matrixd _matrix;
};

}
}

#endif // __VRV_CORE_SHARED_MATRIX_H__
