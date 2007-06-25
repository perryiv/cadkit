
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

namespace VRV {
namespace Core {

class VRV_EXPORT SharedDouble : public vpr::SerializableObject
{
public:
  typedef vpr::SerializableObject BaseClass;

  SharedDouble();

  virtual vpr::ReturnStatus readObject ( vpr::ObjectReader *reader );
  virtual vpr::ReturnStatus writeObject ( vpr::ObjectWriter *writer );

  double data;
};

}
}

#endif // __SHARED_DOUBLE_H__
