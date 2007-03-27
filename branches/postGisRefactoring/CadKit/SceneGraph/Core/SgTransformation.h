
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgTransformation.h: Abstract base transformation class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_TRANSFORMATION_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_TRANSFORMATION_H_

#include "SgNode.h"


namespace CadKit
{
class SG_API SgTransformation : public SgNode
{
protected:

  SgTransformation();
  SgTransformation ( const SgTransformation &t );

  virtual ~SgTransformation();

  SG_DECLARE_NODE ( SgTransformation, 1033859463 );
};
};

#endif
