
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SHARED_UPDATE_DATA_H__
#define __SHARED_UPDATE_DATA_H__

#include "vpr/IO/SerializableObject.h"

class SharedUpdateData : public vpr::SerializableObject
{
public:
  typedef vpr::SerializableObject BaseClass;

  SharedUpdateData ();
  virtual ~SharedUpdateData();

  vpr::ReturnStatus writeObject ( vpr::ObjectWriter *writer );
  vpr::ReturnStatus readObject ( vpr::ObjectReader *reader );

  bool  dataAvailable() const;
  void  dataAvailable( bool b );

private:
  bool _dataAvailable;
};

#endif // __SHARED_UPDATE_DATA_H__
