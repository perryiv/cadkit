
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error checking specification.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_ERROR_CHECKER_H_
#define _CV_ERROR_CHECKER_H_

#include "Usul/Errors/AssertPolicy.h"
#include "Usul/Errors/PrintingPolicy.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/Errors/CompositePolicy.h"

#include <stdexcept>


namespace CV {


///////////////////////////////////////////////////////////////////////////////
//
//  Specify the type of error checking.
//
///////////////////////////////////////////////////////////////////////////////


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

}

#endif // _CV_ERROR_CHECKER_H_
