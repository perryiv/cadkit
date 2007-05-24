
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "SharedDouble.h"

using osgVRJ;

SharedDouble::SharedDouble() : data ( 0.0 )
{
}

vpr::ReturnStatus SharedDouble::readObject ( vpr::ObjectReader *reader )
{
  reader->readDouble ( data );
  return vpr::ReturnStatus::Succeed;
}

vpr::ReturnStatus SharedDouble::writeObject ( vpr::ObjectWriter *writer )
{
  writer->writeDouble( data );
  return vpr::ReturnStatus::Succeed;
}
