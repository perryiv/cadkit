
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgLevelOfDetail: A group node that renders only one child.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_LEVEL_OF_DETAIL_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_LEVEL_OF_DETAIL_H_

#include "SgGroup.h"

namespace CadKit
{
class SG_API SgLevelOfDetail : public SgGroup
{
public:

  SgLevelOfDetail();

protected:

  virtual ~SgLevelOfDetail();

  SG_DECLARE_DYNAMIC_NODE ( SgLevelOfDetail, 1044172772 );
};

};

#endif
