
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Specify the type of error checking.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_ERROR_CHECKER_H_
#define _OSG_TOOLS_ERROR_CHECKER_H_

#include "Usul/Errors/AssertPolicy.h"
#include "Usul/Errors/PrintingPolicy.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/Errors/CompositePolicy.h"
#include "Usul/Errors/DoNothingPolicy.h"

#include <stdexcept>


namespace OsgTools {


#ifdef _DEBUG

  #ifdef _WIN32

    typedef Usul::Errors::CompositePolicy
    <
      Usul::Errors::AssertPolicy, 
      Usul::Errors::ThrowingPolicy < std::runtime_error >
    >
    ErrorChecker;

  #else

    typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorChecker;

  #endif

#else

  typedef Usul::Errors::DoNothingPolicy ErrorChecker;

#endif


}; // namespace OsgTools


#endif // _OSG_TOOLS_ERROR_CHECKER_H_
