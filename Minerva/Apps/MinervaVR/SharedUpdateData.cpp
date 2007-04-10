
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "SharedUpdateData.h"

#include "vpr/IO/ObjectWriter.h"
#include "vpr/IO/ObjectReader.h"

SharedUpdateData::SharedUpdateData () : BaseClass(),
_dataAvailable( false )
{
}

SharedUpdateData::~SharedUpdateData()
{
}

vpr::ReturnStatus SharedUpdateData::writeObject ( vpr::ObjectWriter *writer )
{
  return writer->writeBool ( _dataAvailable );
}

vpr::ReturnStatus SharedUpdateData::readObject ( vpr::ObjectReader *reader )
{
  _dataAvailable = reader->readBool ( );
  return vpr::ReturnStatus();
}

bool  SharedUpdateData::dataAvailable() const
{
  return _dataAvailable;
}

void  SharedUpdateData::dataAvailable( bool b )
{
  _dataAvailable = b;
}
