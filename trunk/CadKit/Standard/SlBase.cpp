
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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

const SlType SlBase::typeSlBase = { "SlBase", NULL, NULL };


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
  return NULL;
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


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this instance is derived from the given type.
//
///////////////////////////////////////////////////////////////////////////////

bool SlType::isDerivedFrom ( const SlType *baseClass ) const
{
  SL_ASSERT ( this && baseClass );

  const SlType *classType = this;

  while ( NULL != classType )
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
  // of the class is abstract or concrete.
  return ( _createFunction ) ? (*_createFunction)() : NULL;
}
