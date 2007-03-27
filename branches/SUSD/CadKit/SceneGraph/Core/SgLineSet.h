
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgLineSet.h: Line set class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_LINE_SET_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_LINE_SET_H_

#include "SgVertexSet.h"

namespace CadKit
{
class SG_API SgLineSet : public SgVertexSet
{
public:

  SgLineSet();

protected:

  virtual ~SgLineSet();

  SG_DECLARE_DYNAMIC_NODE(SgLineSet,0x00001019);
};
};

#endif
