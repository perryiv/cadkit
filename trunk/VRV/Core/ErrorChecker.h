
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error checking specification.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_ERROR_CHECKER_H_
#define _VIRTUAL_REALITY_VIEWER_ERROR_CHECKER_H_

#include "Usul/Errors/AssertPolicy.h"
#include "Usul/Errors/PrintingPolicy.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/Errors/CompositePolicy.h"
#include "Usul/Errors/DoNothingPolicy.h"

#include <stdexcept>


namespace VRV {


///////////////////////////////////////////////////////////////////////////////
//
//  Specify the type of error checking.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG     // Debug/Release

  #ifdef _MSC_VER // VC++/Unix

    typedef Usul::Errors::CompositePolicy
    <
      Usul::Errors::AssertPolicy, 
      Usul::Errors::ThrowingPolicy < std::runtime_error >
    >
    ErrorChecker;

  #else           // VC++/Unix

    #if 0
      typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorChecker;
    #else
      typedef Usul::Errors::PrintingPolicy ErrorChecker;
    #endif

  #endif          // VC++/Unix

#else             // Debug/Release

  typedef Usul::Errors::DoNothingPolicy ErrorChecker;

#endif            // Debug/Release


///////////////////////////////////////////////////////////////////////////////
//
//  Specify the type of warning checking.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG     // Debug/Release

  #ifdef _MSC_VER // VC++/Unix

    typedef Usul::Errors::CompositePolicy
    <
      Usul::Errors::PrintingPolicy,
      Usul::Errors::AssertPolicy
    >
    WarningChecker;

  #else           // VC++/Unix

    typedef Usul::Errors::PrintingPolicy WarningChecker;

  #endif          // VC++/Unix

#else             // Debug/Release

  typedef Usul::Errors::DoNothingPolicy WarningChecker;

#endif            // Debug/Release


}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_ERROR_CHECKER_H_
