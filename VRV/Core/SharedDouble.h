
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
//  Class for sharing a double across all nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SHARED_DOUBLE_H__
#define __SHARED_DOUBLE_H__

#include "VRV/Export.h"

#include "vpr/IO/SerializableObject.h"

namespace vpr { class ObjectReader; class ObjectWriter; }

#if __VPR_version < 1001005
#define RETURN_TYPE vpr::ReturnStatus
#else
#define RETURN_TYPE void
#endif

namespace VRV {
namespace Core {

class VRV_EXPORT SharedDouble : public vpr::SerializableObject
{
public:
  typedef vpr::SerializableObject BaseClass;

  SharedDouble();

  virtual RETURN_TYPE readObject ( vpr::ObjectReader *reader );
  virtual RETURN_TYPE writeObject ( vpr::ObjectWriter *writer );

  double data;
};

}
}

#endif // __SHARED_DOUBLE_H__
