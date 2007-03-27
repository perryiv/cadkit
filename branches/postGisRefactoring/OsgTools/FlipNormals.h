
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor to flip the normals.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_FLIP_NORMALS_H_
#define _OSG_TOOLS_FLIP_NORMALS_H_

#include "osg/Geometry"

#include <algorithm>



namespace OsgTools {


struct FlipNormals
{
  void operator () ( osg::Drawable &drawable )
  {
    // See if it is a geometry.
    osg::Geometry *geometry = dynamic_cast < osg::Geometry * > ( &drawable );
    if ( 0x0 == geometry )
      return;

    // Get the normal array.
    osg::Vec3Array *normals = dynamic_cast < osg::Vec3Array * > ( geometry->getNormalArray() );
    if ( normals )
    {
      // Loop through the normals and negate them.
      std::transform ( normals->begin(), normals->end(), normals->begin(), std::negate<osg::Vec3>() );

      // Rebuild the display lists.
      geometry->dirtyDisplayList();
    }
  }
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_FLIP_NORMALS_H_
