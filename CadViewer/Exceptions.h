
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_EXCEPTIONS_H_
#define _CV_EXCEPTIONS_H_

#include <stdexcept>


namespace CV {
namespace Exceptions {


///////////////////////////////////////////////////////////////////////////////
//
// Exception for corrupt registry.
//
///////////////////////////////////////////////////////////////////////////////

struct CorruptRegistry : public std::runtime_error
{
  CorruptRegistry ( const std::string &what ) : std::runtime_error ( what ){}
};


}; // namespace Exceptions
}; // namespace CV


#endif // _CV_EXCEPTIONS_H_
