
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
//  SlRefBase.cpp: 
//  Inherit from this class to get reference counting members. When the
//  reference number is decremented to 0 the destructor is called.
//  
//  Note: by not having a default constructor we force the client to decide 
//  what the initial reference count is. Typically, it is either 0 or 1.
//
///////////////////////////////////////////////////////////////////////////////

#include "SlPrecompiled.h"
#include "SlRefBase.h"
#include "SlAssert.h"
#include "SlPrint.h"
#include "SlThread.h"

using namespace CadKit;

SL_IMPLEMENT_CLASS(SlRefBase,SlBase);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

SlRefBase::SlRefBase ( const unsigned long &refCount ) : SlBase(), _refCount ( refCount )
{
  SL_PRINT3 ( "SlRefBase::SlRefBase(), this = %X, _refCount = %d, class name = %s\n", this, _refCount, this->getClassName() );
}

#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SlRefBase::~SlRefBase()
{
  SL_PRINT4 ( "SlRefBase::~SlRefBase(), this = %X, _refCount = %d, class name = %s\n", this, _refCount, this->getClassName() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the reference count by one.
//
///////////////////////////////////////////////////////////////////////////////

void SlRefBase::_incrementReferenceCount()
{
  SL_PRINT4 ( "SlRefBase::_incrementReferenceCount(), this = %X, _refCount = %d, class name = %s\n", this, _refCount, this->getClassName() );
  SL_ASSERT ( this );

  // Increment the reference count.
  CadKit::threadSafeIncrement ( _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release the object, delete it if you should.
//
///////////////////////////////////////////////////////////////////////////////

void SlRefBase::_decrementReferenceCount()
{
  SL_PRINT4 ( "SlRefBase::_decrementReferenceCount(), this = %X, _refCount = %d, class name = %s\n", this, _refCount, this->getClassName() );
  SL_ASSERT ( this );

  // Decrement the reference count.
  CadKit::threadSafeDecrement ( _refCount );

  // Delete this instance if the count is now zero.
  if ( 0 == _refCount ) 
    delete this;
}


namespace CadKit {


///////////////////////////////////////////////////////////////////////////////
//
//  This function makes SlRefBase work with SlRefPtr.
//
///////////////////////////////////////////////////////////////////////////////

void _incrementPointerReferenceCount ( SlRefBase *p )
{
  p->_incrementReferenceCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  This function makes SlRefBase work with SlRefPtr.
//
///////////////////////////////////////////////////////////////////////////////

void _decrementPointerReferenceCount ( SlRefBase *p )
{
  p->_decrementReferenceCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Safely reference the pointer.
//
///////////////////////////////////////////////////////////////////////////////

void safeReference ( SlRefBase *p )
{
  if ( p )
    p->_incrementReferenceCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Safely unreference the pointer.
//
///////////////////////////////////////////////////////////////////////////////

void safeDereference ( SlRefBase *p )
{
  if ( p )
    p->_decrementReferenceCount();
}


}; // namespace CadKit
