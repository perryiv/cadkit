
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

  SlQueryPtr ( CadKit::IUnknown *unknown );

  // Assignment.
  SlQueryPtr<T> &operator = ( CadKit::IUnknown *unknown );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Safely query the interface. This function is used below.
//
///////////////////////////////////////////////////////////////////////////////

inline IUnknown *queryInterface ( const unsigned int &iid, IUnknown *unknown )
{
  return ( unknown ) ? unknown->queryInterface ( iid ) : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor. Note what we pass to the base class's constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlQueryPtr<T>::SlQueryPtr ( CadKit::IUnknown *unknown ) : 
  SlRefPtr<T> ( static_cast<T *> ( CadKit::queryInterface ( T::IID, unknown ) ) )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlQueryPtr<T> &SlQueryPtr<T>::operator = ( CadKit::IUnknown *unknown )
{
  // Set this instance from the result of the safe query (which may be null).
  this->setValue ( static_cast<T *> ( CadKit::queryInterface ( T::IID, unknown ) ) );
  return *this;
}


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_QUERY_INTERFACE_POINTER_CLASS_H_
