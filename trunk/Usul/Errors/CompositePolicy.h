
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking class that is a composite of others.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERROR_POLICY_COMPOSITE_H_
#define _USUL_ERROR_POLICY_COMPOSITE_H_


namespace Usul {
namespace Errors {


template < class Check1_, class Check2_ > struct CompositePolicy
{
  CompositePolicy ( const char *filename, unsigned int line, bool state )
  {
    Check1_ ( filename, line, state );
    Check2_ ( filename, line, state );
  }
  CompositePolicy ( unsigned int num, bool state )
  {
    Check1_ ( num, state );
    Check2_ ( num, state );
  }
  template < class Arg_ > CompositePolicy ( unsigned int num, bool state, const Arg_ &arg )
  {
    Check1_ ( num, state, arg );
    Check2_ ( num, state, arg );
  }
  template < class Arg_ > CompositePolicy ( const char *filename, unsigned int line, bool state, const Arg_ &arg )
  {
    Check1_ ( filename, line, state, arg );
    Check2_ ( filename, line, state, arg );
  }
  template < class Int_ > static void bounds ( unsigned int num, Int_ size, Int_ index )
  {
    Check1_::bounds ( num, size, index );
    Check2_::bounds ( num, size, index );
  }
};


} // namespace Errors
} // namespace Usul


#endif // _USUL_ERROR_POLICY_COMPOSITE_H_
