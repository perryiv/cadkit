
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_QUERY_INTERFACE_POINTER_CLASS_H_
