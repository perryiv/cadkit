
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SMART_POINTERS_EXCEPTION_CLASSES_H_
#define _USUL_SMART_POINTERS_EXCEPTION_CLASSES_H_

#include <stdexcept>


namespace Usul {
namespace Pointers {
namespace Exceptions {


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for smart-pointer exceptions.
//
///////////////////////////////////////////////////////////////////////////////

struct Exception : public std::runtime_error
{
  Exception ( const std::string &what ) : std::runtime_error ( what ){}
};


///////////////////////////////////////////////////////////////////////////////
//
//  Exception for a null pointer.
//
///////////////////////////////////////////////////////////////////////////////

struct NullPointer : public Exception
{
  NullPointer ( const std::string &what ) : Exception ( what ){}
};


///////////////////////////////////////////////////////////////////////////////
//
//  Exception for a failed interface query.
//
///////////////////////////////////////////////////////////////////////////////

struct NoInterface : public NullPointer
{
  NoInterface ( const std::string &what ) : NullPointer ( what ){}
};


} // namespace Exceptions
} // namespace Pointers
} // namespace Usul


#endif // _USUL_SMART_POINTERS_EXCEPTION_CLASSES_H_
