
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Write XML.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _NURBS_FOUNDATION_LIBRARY_WRITE_XML_H_
#define _NURBS_FOUNDATION_LIBRARY_WRITE_XML_H_

#include "NFL/Core/Splines/Spline.h"

#include <iosfwd>
#include <string>


namespace NFL {
namespace Core {
namespace Write {


// Write spline as XML.
NURBS_SPLINE_EXPORT void xml ( NFL::Core::Splines::Spline &s, std::ostream &out, std::string &indent = std::string ( "  " ) );


} // namespace Write
} // namespace Core
} // namespace NFL


#endif //_NURBS_FOUNDATION_LIBRARY_WRITE_XML_H_
