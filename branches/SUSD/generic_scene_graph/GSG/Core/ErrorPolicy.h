
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error policies.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_ERROR_POLICIES_H_
#define _GENERIC_SCENE_GRAPH_CORE_ERROR_POLICIES_H_

#include "GSG/Core/Assert.h"


namespace GSG {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Error policies that assert.
//
///////////////////////////////////////////////////////////////////////////////

struct AssertBoundsChecker
{
  AssertBoundsChecker ( unsigned long size, unsigned long index )
  {
    GSG_ASSERT ( index >= 0 && index < size );
  }
};
struct AssertErrorChecker
{
  AssertErrorChecker ( bool state )
  {
    GSG_ASSERT ( state );
  }
};
struct AssertNullChecker
{
  template < class T > static void check ( const T *ptr )
  {
    GSG_ASSERT ( ptr );
  }
};


}; // namespace Detail
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_ERROR_POLICIES_H_
