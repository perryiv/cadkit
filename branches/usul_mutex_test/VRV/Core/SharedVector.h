
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach, Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for sharing an osg::Matrix across all nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_CORE_SHARED_VECTOR_H__
#define __VRV_CORE_SHARED_VECTOR_H__

#include "VRV/Export.h"

#include "vpr/IO/SerializableObject.h"

#include "vector"

namespace vpr { class ObjectReader; class ObjectWriter; }

#if __VPR_version < 1001005
#define RETURN_TYPE vpr::ReturnStatus
#else
#define RETURN_TYPE void
#endif


namespace VRV {
namespace Core {

template < class T >
class SharedVector : public vpr::SerializableObject
{
public:
  typedef std::vector < T > VectorType;
  typedef typename VectorType::size_type SizeType;
  typedef typename VectorType::const_iterator ConstIterator;
  typedef T ValueType;
  typedef vpr::SerializableObject BaseClass;

  SharedVector();
  virtual ~SharedVector();

  /// Read/Write the object.
  virtual RETURN_TYPE readObject ( vpr::ObjectReader *reader );
  virtual RETURN_TYPE writeObject ( vpr::ObjectWriter *writer );

  /// Get/Set the vector.
  const VectorType&       vector() const            { return _vector; }
  void                    vector( const VectorType& v ) { _vector = v ; }

private:
  VectorType _vector;
}; // __VRV_CORE_SHARED_VECTOR_H__
};
};
#endif