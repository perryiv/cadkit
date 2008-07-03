
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POINTSET_RECORDS_H__
#define __POINTSET_RECORDS_H__

#include "Usul/Types/Types.h"

#include "osg/Vec3"

//namespace osg { class Node; }

namespace PointSetRecords 
{
   
    namespace Record
    {
      typedef Usul::Types::Uint32 Type;
      const Type OCTREE_HEADER      ( 1 );
      const Type OCTREE_NODE        ( 2 );
      const Type LOD_DEFINITIONS    ( 3 );
      const Type POINTS             ( 4 );
      const Type CHILDREN           ( 5 );
      const Type COLORS             ( 6 );
    }
   
}


#endif // __POINTSET_RECORDS_H__
