
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

// I do not like to putting this here, as I view Standard and Interfaces to 
// be equally low-level. However, when I forget to include IUnknown.h before 
// this header, I get a VC++ internal compiler error (which is doesn't print 
// any kind of clue as to the source of the problem). Since IUnknown.h is 
// entirely inline this should not create a dependency problem.
#include "Interfaces/IUnknown.h"


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
