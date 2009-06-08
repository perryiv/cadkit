
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Triangle exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_EXCEPTIONS_H_
#define _OPEN_SCENE_GRAPH_TOOLS_EXCEPTIONS_H_

#include "OsgTools/Export.h"

#include <stdexcept>
#include <string>


namespace OsgTools {
namespace Triangles {
namespace Exceptions {


///////////////////////////////////////////////////////////////////////////////
//
//  Two or more of the triangle's vertices are equal.
//
///////////////////////////////////////////////////////////////////////////////

struct OSG_TOOLS_EXPORT TriangleVerticesEqual : public std::invalid_argument
{
  typedef std::invalid_argument BaseClass;
  TriangleVerticesEqual ( const std::string &message ) : BaseClass ( message )
  {
  }
};


} // namespace Exceptions
} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_EXCEPTIONS_H_
