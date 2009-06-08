
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
//  DbJtTargets: Jupiter database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbJtPrecompiled.h"
#include "DbJtTargets.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <algorithm>
#endif

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbJtTargets, SlRefBase );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbJtTargets, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTargets::DbJtTargets() : SlRefBase ( 0 ),
  _interfaces ( new Interfaces )
{
  SL_PRINT2 ( "In DbJtTargets::DbJtTargets(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTargets::~DbJtTargets()
{
  SL_PRINT2 ( "In DbJtTargets::~DbJtTargets(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbJtTargets::queryInterface ( unsigned long iid )
{
  SL_PRINT2 ( "In DbJtTargets::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IInterfaceGroup::IID:
    return static_cast<IInterfaceGroup *>(this);
  default:
    return NULL;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the interface.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtTargets::addInterface ( IUnknown *unknown )
{
  SL_PRINT3 ( "In DbJtTargets::addInterface(), this = %X, unknown = %X\n", this, unknown );
  SL_ASSERT ( unknown );

  // Add it to the end of the sequence. This will automatically reference it.
  _interfaces->push_back ( unknown );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of interfaces.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int DbJtTargets::getNumInterfaces() const
{
  SL_PRINT2 ( "In DbJtTargets::getNumInterfaces(), this = %X\n", this );
  return _interfaces->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the interface. Returns the number of occurances removed.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int DbJtTargets::removeInterface ( IUnknown *ptr )
{
  SL_PRINT3 ( "In DbJtTargets::removeInterface(), this = %X, unknown = %X\n", this, ptr );

  // Grab the current size.
  unsigned int size = _interfaces->size();

  // Irix/CC insists that we wrap this.
  SlRefPtr<IUnknown> unknown ( ptr );

  // Remove the interface if we find it.
  Interfaces::iterator begin = _interfaces->begin();
  Interfaces::iterator last = std::remove ( begin, _interfaces->end(), unknown );

  // We need to release detach the ref-pointer and release the interface
  // without deleting it. The caller may have passed an interface with a 
  // reference count of zero.
  //unknown.makeNull();
  // TODO, need to make SlRefBase::unrefNoDelete() and 
  // IUnknown::unrefNoDelete(), unless there is another way...

  // Resize the sequence (the docs say that std::remove() does not resize).
  _interfaces->resize ( last - begin );

  // Return the number removed.
  return size - _interfaces->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all interfaces.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtTargets::removeAllInterfaces()
{
  SL_PRINT2 ( "In DbJtTargets::removeAllInterfaces(), this = %X\n", this );

  // Clear the sequence.
  _interfaces->clear();
}
