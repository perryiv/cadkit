
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

#ifndef __VRV_CORE_SHARED_MAP_H__
#define __VRV_CORE_SHARED_MAP_H__

#include "VRV/Export.h"

#include "vpr/IO/SerializableObject.h"

#include "map"

namespace vpr { class ObjectReader; class ObjectWriter; }

#if __VPR_version < 1001005
#define RETURN_TYPE vpr::ReturnStatus
#else
#define RETURN_TYPE void
#endif


namespace VRV {
namespace Core {

template < class Key, class Value >
class SharedMap : public vpr::SerializableObject
{
public:
  typedef std::map < Key, Value > MapType;
  typedef typename MapType::size_type SizeType;
  typedef typename MapType::const_iterator ConstIterator;
  typedef vpr::SerializableObject BaseClass;

  SharedMap();
  virtual ~SharedMap();

  /// Read/Write the object.
  virtual RETURN_TYPE readObject ( vpr::ObjectReader *reader );
  virtual RETURN_TYPE writeObject ( vpr::ObjectWriter *writer );

  /// Get/Set the vector.
  const MapType&       vector() const            { return _map; }
  void                    vector( const MapType& m ) { _map = m ; }

private:
  MapType _map;
}; // __VRV_CORE_SHARED_MAP_H__
};
};
#endif