
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Typical error checking.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERROR_CHECKER_H_
#define _USUL_ERROR_CHECKER_H_

#include "Usul/Errors/AssertPolicy.h"
#include "Usul/Errors/PrintingPolicy.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/Errors/CompositePolicy.h"
#include "Usul/Errors/DoNothingPolicy.h"

#include <stdexcept>


namespace Usul {
namespace Errors {


#ifdef _DEBUG

  #ifdef _WIN32

    typedef Usul::Errors::CompositePolicy
    <
      Usul::Errors::AssertPolicy, 
      Usul::Errors::ThrowingPolicy < std::runtime_error >
    >
    Checker;

  #else

    typedef Usul::Errors::ThrowingPolicy < std::runtime_error > Checker;

  #endif

#else

  typedef Usul::Errors::DoNothingPolicy Checker;

#endif


}; // namespace Errors
}; // namespace Usul


#endif // _USUL_ERROR_CHECKER_H_
