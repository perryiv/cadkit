
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

// I do not like to putting this here, as I view Standard and Interfaces to 
// be equally low-level. However, when I forget to include IUnknown.h before 
// this header, I get a VC++ internal compiler error (which it doesn't print 
// any kind of clue as to the source of the problem). Since IUnknown.h is 
// entirely inline this should not create a dependency problem.
// TODO, figure out a better solution.
//#include "Interfaces/IUnknown.h"


namespace CadKit{
namespace Private {


///////////////////////////////////////////////////////////////////////////////
//
//  Safely query for the interface. The template parameter NoInterfacePolicy
//  defines what happens when the interface is not found.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  typename InterfaceID, 
  class InterfaceType, 
  class NoInterfacePolicy
>
inline InterfaceType *queryInterface ( InterfaceID iid, InterfaceType *unknown, NoInterfacePolicy &policy )
{
  // Overload this for specific types.
  InterfaceType *answer = CadKit::queryInterface ( iid, unknown );

  // Check the policy.
  policy ( answer );

  // Return the answer.
  return answer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Policy class that does nothing.
//
///////////////////////////////////////////////////////////////////////////////

struct DoNothing
{
  void operator() ( const void * ){}
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
  class OldInterfaceType = CadKit::IUnknown,
  class NoInterfacePolicy = Private::DoNothing
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
    SlRefPtr<NewInterfaceType> ( static_cast<NewInterfaceType *> ( Private::queryInterface ( NewInterfaceType::IID, ptr, NoInterfacePolicy() ) ) )
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  SlQueryPtr &operator = ( OldInterfaceType *ptr )
  {
    // Set this instance from the result of the safe query (which may be null).
    this->setValue ( static_cast<NewInterfaceType *> ( Private::queryInterface ( NewInterfaceType::IID, ptr, NoInterfacePolicy() ) ) );
    return *this;
  }
};


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_QUERY_INTERFACE_POINTER_CLASS_H_
