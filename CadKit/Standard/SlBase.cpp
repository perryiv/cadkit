
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlBase.cpp: Runtime type info (rtti) and dynamic creation behavior.
//
///////////////////////////////////////////////////////////////////////////////

#include "SlPrecompiled.h"
#include "SlBase.h"
#include "SlAssert.h"
#include "SlPrint.h"

using namespace CadKit;

//
// Note: The implementation of some of the below functions is an example of 
// what the macros put in your class.
//

///////////////////////////////////////////////////////////////////////////////
//
//  Static member(s).
//
///////////////////////////////////////////////////////////////////////////////

const SlType SlBase::typeSlBase = { "SlBase", 0x0, 0x0 };


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

SlBase::SlBase()
{
  SL_PRINT2 ( "In SlBase::SlBase(), this = %X\n", this );
}

#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Return the class name.
//
///////////////////////////////////////////////////////////////////////////////

const char *SlBase::getClassName() const
{
  SL_ASSERT ( this );

  const SlType *rtc = this->getType();
  return rtc->_className;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the runtime object of the base class. If there's no base class
//  (like in this case) then this returns NULL.
//
///////////////////////////////////////////////////////////////////////////////

const SlType *SlBase::getBaseClassType()
{
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the runtime object of the class.
//
///////////////////////////////////////////////////////////////////////////////

const SlType *SlBase::getClassType()
{
  return SL_CLASS_TYPE ( SlBase );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the id the class.
//
///////////////////////////////////////////////////////////////////////////////

SlUint32 SlBase::getClassTypeId()
{
  return SL_CLASS_ID ( SlBase );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the runtime object of this class. If there's no base class
//  then this returns NULL.
//
///////////////////////////////////////////////////////////////////////////////

const SlType *SlBase::getType() const
{
  return SL_CLASS_TYPE ( SlBase );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the id of this class. If there's no base class
//  then this returns NULL.
//
///////////////////////////////////////////////////////////////////////////////

SlUint32 SlBase::getTypeId() const
{
  return SL_CLASS_ID ( SlBase );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this instance is of the given type or derived from it.
//
///////////////////////////////////////////////////////////////////////////////

bool SlBase::isOfType ( const SlType *classType ) const
{
  SL_ASSERT ( this && classType );

  const SlType *rtc = this->getType();
  return rtc->isDerivedFrom ( classType );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this instance is of the given type.
//
///////////////////////////////////////////////////////////////////////////////

bool SlBase::isOfExactType ( const SlType *classType ) const
{
  SL_ASSERT ( this && classType );

  return ( classType == this->getType() );
}
