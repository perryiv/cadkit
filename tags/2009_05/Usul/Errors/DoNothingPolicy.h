
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking class that does nothing, and (hopefully) gets optimized away.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERROR_POLICY_DO_NOTHING_H_
#define _USUL_ERROR_POLICY_DO_NOTHING_H_


namespace Usul {
namespace Errors {


struct DoNothingPolicy
{
  DoNothingPolicy ( const char *, unsigned int, bool )
  {
  }
  DoNothingPolicy ( unsigned int, bool )
  {
  }
  template < class Arg_ > DoNothingPolicy ( unsigned int, bool, const Arg_ & )
  {
  }
  template < class Arg_ > DoNothingPolicy ( const char *, unsigned int, bool, const Arg_ & )
  {
  }
  template < class Int_ > static void bounds ( unsigned int, Int_, Int_ )
  {
  }
};


} // namespace Errors
} // namespace Usul


#endif // _USUL_ERROR_POLICY_DO_NOTHING_H_
