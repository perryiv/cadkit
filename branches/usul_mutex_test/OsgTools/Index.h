
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions related to indices.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_INDEX_FUNCTIONS_H_
#define _OSG_TOOLS_INDEX_FUNCTIONS_H_

#include "osg/PrimitiveSet"

#include <stdexcept>


namespace OsgTools {


///////////////////////////////////////////////////////////////////////////////
//
//  Returns the index multiplier. For example, the 4th triangle starts at 
//  the 4*3=12th vertex.
//
///////////////////////////////////////////////////////////////////////////////

template < class ModeType > unsigned int polygonIndexMultiplier ( ModeType mode )
{
  switch ( mode )
  {
  case osg::PrimitiveSet::LINE_LOOP:
  case osg::PrimitiveSet::LINE_STRIP:
  case osg::PrimitiveSet::LINES:
    return 2;
  case osg::PrimitiveSet::TRIANGLE_FAN:
  case osg::PrimitiveSet::TRIANGLE_STRIP:
  case osg::PrimitiveSet::TRIANGLES:
    return 3;
  case osg::PrimitiveSet::QUAD_STRIP:
  case osg::PrimitiveSet::QUADS:
    return 4;
  case osg::PrimitiveSet::POINTS:
    return 1;
  case osg::PrimitiveSet::POLYGON:
  default:
    throw std::runtime_error ( "Error 3787188959:, unsupported primitive-set mode" );
    break;
  }
}


}; // namespace OsgTools


#endif // _OSG_TOOLS_INDEX_FUNCTIONS_H_
