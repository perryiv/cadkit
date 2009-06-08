
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlType.cpp: For runtime type info (rtti).
//
///////////////////////////////////////////////////////////////////////////////

#include "SlPrecompiled.h"
#include "SlType.h"
#include "SlAssert.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this instance is derived from the given type.
//
///////////////////////////////////////////////////////////////////////////////

bool SlType::isDerivedFrom ( const SlType *baseClass ) const
{
  SL_ASSERT ( this && baseClass );

  const SlType *classType = this;

  while ( 0x0 != classType )
  {
    if ( classType == baseClass ) return true;
    classType = classType->_baseClass;
  }

  return false; // No match.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a new instance of the class that this runtime class is 
//  associated with, or null if there is not create function.
//
///////////////////////////////////////////////////////////////////////////////

SlBase *SlType::createObject() const
{
  SL_ASSERT ( this );

  // Note: Do not assert here. The client can use this function to determine 
  // if the class is abstract or concrete.
  return ( _createFunction ) ? (*_createFunction)() : 0x0;
}
