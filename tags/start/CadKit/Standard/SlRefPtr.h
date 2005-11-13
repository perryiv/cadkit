
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
//  SlRefPtr.h: 
//  Pointer class that handles reference counting. You have to overload 
//  CadKit::_incrementPointerReferenceCount() and
//  CadKit::_decrementPointerReferenceCount() for the pointer type used.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_SMART_REFERENCE_POINTER_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_SMART_REFERENCE_POINTER_CLASS_H_


namespace CadKit
{
template<class T> class SlRefPtr
{
public:

  SlRefPtr();
  SlRefPtr ( const SlRefPtr<T> &p );
  SlRefPtr ( T *p );
  ~SlRefPtr();

  /// Make the internal pointer null. This is different from setValue() in 
  /// that it will not release the pointer.
  void              makeNull() { _p = 0x0; }

  /// Get the value. We return a non-const copy of the internal pointer because 
  /// there is no way the client can use that pointer to change the internal 
  /// state of this instance.
  T *               getValue() const { return _p; }

  /// Check internal status.
  bool              isNull()  const { return 0x0 == _p; }
  bool              isValid() const { return 0x0 != _p; }

  /// Typecast operators.
  operator T *()             { return _p; }
  operator const T *() const { return _p; }

  /// Misc operators.
  const T *         operator -> () const { return _p; }
  T *               operator -> ()       { return _p; }

  const T &         operator *  () const { return *_p; }
  T &               operator *  ()       { return *_p; }

  bool              operator !  ()       { return 0x0 == _p; }

  SlRefPtr<T> &     operator =  ( const SlRefPtr<T> &p ) { this->setValue ( p ); return *this; }
  SlRefPtr<T> &     operator =  ( T *p )                 { this->setValue ( p ); return *this; }

  bool              operator == ( const SlRefPtr<T> &p )               { return _p == p._p; }
  bool              operator == ( const T *p )                         { return _p == p; }
  friend bool       operator == ( const T *p1, const SlRefPtr<T> &p2 ) { return p1 == p2._p; }

  bool              operator != ( const SlRefPtr<T> &p )               { return _p != p._p; }
  bool              operator != ( const T *p )                         { return _p != p; }
  friend bool       operator != ( const T *p1, const SlRefPtr<T> &p2 ) { return p1 != p2._p; }

  /// Set the internal pointer.
  void              setValue ( const SlRefPtr<T> &p );
  void              setValue ( T *p );

protected:

  T *_p;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlRefPtr<T>::SlRefPtr() :
  _p ( 0x0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlRefPtr<T>::SlRefPtr ( const SlRefPtr &p ) :
  _p ( p._p )
{
  if ( _p )
    CadKit::_incrementPointerReferenceCount ( _p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlRefPtr<T>::SlRefPtr ( T *p ) :
  _p ( p )
{
  if ( _p )
    CadKit::_incrementPointerReferenceCount ( _p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlRefPtr<T>::~SlRefPtr()
{
  if ( _p )
    CadKit::_decrementPointerReferenceCount ( _p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void SlRefPtr<T>::setValue ( T *p )
{
  // If the given pointer is the same address then just return.
  if ( _p == p )
    return;

  // Make a copy of the current one.
  T *old = _p;

  // Copy the given pointer.
  _p = p;

  // If the given pointer is not null then reference it.
  if ( _p )
    CadKit::_incrementPointerReferenceCount ( _p );

  // If the old one is not null then release it. Make sure we do this last 
  // because the given pointer "p" could be a child (indirect or direct) of 
  // "old". If we release "old" before we reference "p" then "p" may get 
  // deleted when it should not have.
  if ( old )
    CadKit::_decrementPointerReferenceCount ( old );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void SlRefPtr<T>::setValue ( const SlRefPtr<T> &p )
{
  // Call the other one.
  this->setValue ( p._p );
}


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_SMART_REFERENCE_POINTER_CLASS_H_
