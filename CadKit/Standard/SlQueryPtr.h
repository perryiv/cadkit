
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
//  SlQueryPtr.h: Works like CComQIPtr.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_QUERY_INTERFACE_POINTER_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_QUERY_INTERFACE_POINTER_CLASS_H_

#include "SlRefPtr.h"


namespace CadKit
{
template <class T> class SlQueryPtr : public SlRefPtr<T>
{
public:

  SlQueryPtr ( const unsigned long &iid, CadKit::IUnknown *unknown );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor. Note we call base class's default constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlQueryPtr<T>::SlQueryPtr 
  ( const unsigned long &iid, CadKit::IUnknown *unknown ) : SlRefPtr<T>()
{
  if ( unknown )
    this->setValue ( static_cast<T *> ( unknown->queryInterface ( iid ) ) );
}


// So that SlRefPtr works with type T.
template <class T> inline void _incrementPointerReferenceCount ( T *p ) { p->ref(); }
template <class T> inline void _decrementPointerReferenceCount ( T *p ) { p->unref(); }


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_QUERY_INTERFACE_POINTER_CLASS_H_
