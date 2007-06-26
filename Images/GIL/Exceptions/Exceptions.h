
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Possible exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_GIL_EXCEPTION_CLASSES_H_
#define _IMAGES_GIL_EXCEPTION_CLASSES_H_

#include <stdexcept>
#include <string>


namespace Images {
namespace GIL {
namespace Exceptions {


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#define MAKE_EXCEPTION_CLASS(class_name)\
struct class_name : std::runtime_error\
{\
  class_name ( const std::string &message ) : std::runtime_error ( message )\
  {\
  }\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Possible exceptions.
//
///////////////////////////////////////////////////////////////////////////////

MAKE_EXCEPTION_CLASS ( CommandLineException );


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

#undef MAKE_EXCEPTION_CLASS


} // namespace Exceptions
} // namespace GIL
} // namespace Images


#endif // _IMAGES_GIL_EXCEPTION_CLASSES_H_
