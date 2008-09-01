
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

#include "NFL/Core/Write/XML.h"

#include "GN/Write/XML.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespace.
//
///////////////////////////////////////////////////////////////////////////////

namespace NFL {
namespace Core {
namespace Write {


///////////////////////////////////////////////////////////////////////////////
//
//  Write spline in XML format.
//
///////////////////////////////////////////////////////////////////////////////

void xml ( NFL::Core::Splines::Spline &s, std::ostream &out, std::string &indent )
{
  GN::Write::xml ( s.spline(), indent, out );
}


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace.
//
///////////////////////////////////////////////////////////////////////////////

} // namespace Write
} // namespace Core
} // namespace NFL