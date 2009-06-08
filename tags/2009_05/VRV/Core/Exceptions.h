
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_CORE_EXCEPTIONS_H__
#define __VRV_CORE_EXCEPTIONS_H__

#include <stdexcept>

namespace VRV {
namespace Core {
namespace Exceptions {


///////////////////////////////////////////////////////////////////////////////
//
// Exception for invalid user input.
//
///////////////////////////////////////////////////////////////////////////////

struct UserInput : public std::invalid_argument
{
  UserInput ( const std::string &what ) : std::invalid_argument ( what ){}
};


} // namespace Exceptions
} // namespace Core
} // namespace CV


#endif // __VRV_CORE_EXCEPTIONS_H__
