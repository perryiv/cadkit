
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlQueryPtr: Works like CComQIPtr.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_QUERY_INTERFACE_POINTER_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_QUERY_INTERFACE_POINTER_CLASS_H_

#include "SlRefPtr.h"


namespace CadKit{
namespace Private {


///////////////////////////////////////////////////////////////////////////////
//
//  Policy class that does nothing.
//
///////////////////////////////////////////////////////////////////////////////

struct QueryPtrDefaultPolicy
{
  QueryPtrDefaultPolicy(){}
  template < class Argument > void operator() ( bool state, const Argument &argument ){}
};


}; // namespace Private


///////////////////////////////////////////////////////////////////////////////
//
//  SlQueryPtr class declaration.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class NewInterfaceType, 
  class NoInterfacePolicy = CadKit::Private::QueryPtrDefaultPolicy,
  class OldInterfaceType = CadKit::IUnknown
>
class SlQueryPtr : public SlRefPtr<NewInterfaceType>
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor. Note what we pass to the base class's constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  SlQueryPtr ( OldInterfaceType *ptr ) : 
    SlRefPtr<NewInterfaceType> ( static_cast<NewInterfaceType *> ( CadKit::queryInterface ( NewInterfaceType::IID, ptr ) ) )
  {
    // Check the policy.
    NoInterfacePolicy() ( this->isValid(), "Interface not found" );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor. Takes a failure message. Note what we pass to the base 
  //  class's constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Argument > SlQueryPtr ( OldInterfaceType *ptr, const Argument &argument ) : 
    SlRefPtr<NewInterfaceType> ( static_cast<NewInterfaceType *> ( CadKit::queryInterface ( NewInterfaceType::IID, ptr ) ) )
  {
    // Check the policy.
    NoInterfacePolicy() ( this->isValid(), argument );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  SlQueryPtr &operator = ( OldInterfaceType *ptr )
  {
    // Set this instance from the result of the safe query (which may be null).
    this->setValue ( static_cast<NewInterfaceType *> ( CadKit::queryInterface ( NewInterfaceType::IID, ptr ) ) );

    // Check the policy.
    NoInterfacePolicy() ( this->isValid(), "Interface not found" );

    return *this;
  }
};


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_QUERY_INTERFACE_POINTER_CLASS_H_
