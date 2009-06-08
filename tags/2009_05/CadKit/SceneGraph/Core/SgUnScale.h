
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgUnScale.h: Sets the scale in the modelview matrix to 1.0.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_UNSCALE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_UNSCALE_H_

#include "SgTransformation.h"


namespace CadKit
{
class SG_API SgUnScale : public SgTransformation
{
public:

  enum // Possible flags.
  {
    PROCESS_ROTATIONS    = 0x00000001, // Account for rotations in the matrix when getting the scale.
    ASSUME_UNIFORM_SCALE = 0x00000002, // Assume the matrix has a uniform scale.
  };

  SgUnScale();
  SgUnScale ( const SgUnScale &unscale );

protected:

  unsigned int _unScaleFlags;

  virtual ~SgUnScale();

  SL_DECLARE_BITMASK_FUNCTIONS ( Flags, unsigned int, _unScaleFlags );
  SG_DECLARE_DYNAMIC_NODE ( SgUnScale, 1033951260 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_UNSCALE_H_
