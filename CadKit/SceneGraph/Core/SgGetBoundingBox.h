
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGetBoundingBox: The bounding box class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_GET_BOUNDING_BOX_RENDERER_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_GET_BOUNDING_BOX_RENDERER_H_

#include "SgVisitor.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlBoundingBox.h"
# include "Standard/SlMatrix44.h"
#endif


namespace CadKit
{
class SG_API SgGetBoundingBox : public SgVisitor
{
public:

  SgGetBoundingBox();
  virtual ~SgGetBoundingBox();

  // Return the bounding box. Have to call a visit() function first.
  const SlBoundingBoxf &  getBBox() const { return _bbox; }
  SlBoundingBoxf &        getBBox()       { return _bbox; }

  // Call this to reset the bounding box.
  void                    invalidateBBox() { _bbox.init(); }

  // Set the bounding box. Probably don't need this.
  void                    setBBox ( const SlBoundingBoxf &bbox ) { _bbox = bbox; }

  // Set the matrix.
  void                    setIdentityMatrix() { _M.identity(); }
  void                    setMatrix ( const SlMatrix44f &M ) { _M = M; }

  // The visit functions.
  SG_DECLARE_ALL_VISITOR_FUNCTIONS;

protected:

  SlMatrix44f _M;
  SlBoundingBoxf _bbox;

  SG_DECLARE_DYNAMIC_VISITOR ( SgGetBoundingBox, 0x00001013 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_GET_BOUNDING_BOX_RENDERER_H_
