
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
//  SlRefBase.h: 
//  Inherit from this class to get reference counting members. When the
//  reference number is decremented to 0 the destructor is called.
//  
//  Note: by not having a default constructor we force the client to decide 
//  what the initial reference count is. Typically, it is either 0 or 1.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_REFERENCE_OBJECT_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_REFERENCE_OBJECT_CLASS_H_

#include "SlBase.h"
#include "SlRefPtr.h"


namespace CadKit
{
class SlRefBase : public SlBase
{
public:

  /// Get the reference count.
  unsigned long   getRefCount() const { return _refCount; }

  /// Increment/decrement the reference count. These functions break the 
  /// naming convention of using "_" to prefix protected/private members. 
  /// I do this because:
  /// 1. I want the client to overload these and pick names that are intuitive 
  ///    for the usage. For example, ref()/unref() for a scene graph, and 
  ///    AddRef()/Release() for a COM object.
  /// 2. I want these to be callable from _incrementPointerReferenceCount() 
  ///    and _decrementPointerReferenceCount(), which are declared in namespace
  ///    CadKit. The client can overload these functions so that SlRefPtr will
  ///    work with their class.
  void            _incrementReferenceCount();
  void            _decrementReferenceCount();

protected:

  unsigned long _refCount;

  #ifdef _DEBUG
  SlRefBase ( const unsigned long &refCount );
  #else
  SlRefBase ( const unsigned long &refCount ) : SlBase(), _refCount ( refCount ){}
  #endif

  virtual ~SlRefBase();

  SL_DECLARE_CLASS(SlRefBase,0x00000002);
};


/// These are for class SlRefPtr. The client can overload these functions to
/// use SlRefPtr with another kind of pointer (like IUnknown). By making 
/// SlRefPtr call these global functions instead of it's contained pointer's
/// ref/unref (or perhaps AddRef/Release) functions, we decouple it from the 
/// API of it's contained pointer.
inline void _incrementPointerReferenceCount ( SlRefBase *p ) { p->_incrementReferenceCount(); }
inline void _decrementPointerReferenceCount ( SlRefBase *p ) { p->_decrementReferenceCount(); }


}; // namespace CadKit


// Declare a typecast for SlRefPtr. Put this in your class declaration.
#define SL_DECLARE_REFCOUNT_TYPE(class_name) \
  public: \
  typedef SlRefPtr < class_name > Ptr;


#endif // _CADKIT_STANDARD_LIBRARY_REFERENCE_OBJECT_CLASS_H_
