
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convert other primitives (currently only tri-strips) to triangles.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_UTILITIES_CONVERT_TO_TRIANGLES_H__
#define __OSG_TOOLS_UTILITIES_CONVERT_TO_TRIANGLES_H__

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"


namespace OsgTools {
namespace Utilities {

class OSG_TOOLS_EXPORT ConvertToTriangles
{
public:
  
  ConvertToTriangles();
  ~ConvertToTriangles();
  void operator () ( osg::Node * );
  
protected:
  
  void          _convert ( osg::Geode * );
  
private:
  
  ConvertToTriangles ( const ConvertToTriangles & );
  ConvertToTriangles &operator = ( const ConvertToTriangles & );
};

} // namespace Utilities 
} // namespace OsgTools


#endif // __OSG_TOOLS_UTILITIES_CONVERT_TO_TRIANGLES_H__
