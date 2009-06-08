///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to work with lods.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_LOD_H_
#define _OSG_TOOLS_LOD_H_

#include "OsgTools/Export.h"
#include "OsgTools/Configure/OSG.h"
#include "OsgTools/Declarations.h"


namespace OsgTools {


struct OSG_TOOLS_EXPORT Lod
{
  // Set the lod center and ranges.
  static void setCenterAndRanges ( float maxDistanceFactor, float lastRangeMax, osg::LOD *lod );
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_LOD_H_
