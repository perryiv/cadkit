
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SHARED_DOUBLE_H__
#define __SHARED_DOUBLE_H__

#include "Export.h"

#include "vpr/IO/SerializableObject.h"
#include "vpr/IO/ObjectReader.h"
#include "vpr/IO/ObjectWriter.h"

namespace osgVRJ {

class OSG_VRJ_EXPORT SharedDouble : public vpr::SerializableObject
{
public:
  SharedDouble();

  virtual vpr::ReturnStatus readObject ( vpr::ObjectReader *reader );
  virtual vpr::ReturnStatus writeObject ( vpr::ObjectWriter *writer );

  double data;
};

}

#endif // __SHARED_DOUBLE_H__
