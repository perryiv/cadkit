
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error checking specification.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_ERROR_CHECKER_H_
#define _FOX_TOOLS_ERROR_CHECKER_H_

#include "Usul/Errors/AssertPolicy.h"
#include "Usul/Errors/PrintingPolicy.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/Errors/CompositePolicy.h"
#include "Usul/Errors/DoNothingPolicy.h"

#include <stdexcept>


namespace FoxTools {
namespace Errors {


///////////////////////////////////////////////////////////////////////////////
//
//  Specify the type of error checking.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

  #ifdef _MSC_VER

    typedef Usul::Errors::CompositePolicy
    <
      Usul::Errors::AssertPolicy, 
      Usul::Errors::ThrowingPolicy < std::runtime_error >
    >
    ErrorChecker;

  #else

    typedef Usul::Errors::CompositePolicy
    <
      Usul::Errors::PrintingPolicy, 
      Usul::Errors::ThrowingPolicy < std::runtime_error >
    >
    ErrorChecker;

  #endif

#else

  typedef Usul::Errors::DoNothingPolicy ErrorChecker;

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Specify the type of warning checking.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

  typedef Usul::Errors::CompositePolicy
  <
    Usul::Errors::PrintingPolicy,
    Usul::Errors::AssertPolicy
  >
  WarningChecker;

#else

  typedef Usul::Errors::PrintingPolicy WarningChecker;

#endif


}; // namespace Errors
}; // namespace FoxTools


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to make using the above easier.
//
///////////////////////////////////////////////////////////////////////////////

#define FOX_TOOLS_ERROR_CHECKER(exp)   FoxTools::Errors::ErrorChecker   ( __FILE__, __LINE__, ( exp ) );
#define FOX_TOOLS_WARNING_CHECKER(exp) FoxTools::Errors::WarningChecker ( __FILE__, __LINE__, ( exp ) );


#endif // _FOX_TOOLS_ERROR_CHECKER_H_
