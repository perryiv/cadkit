
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgAttribute.h: Base attribute class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_ATTRIBUTE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_ATTRIBUTE_H_

#include "SgNode.h"


namespace CadKit
{
class SG_API SgAttribute : public SgNode
{
protected:

  SgAttribute();
  SgAttribute ( const SgAttribute &attribute );
  virtual ~SgAttribute();

  SG_DECLARE_NODE ( SgAttribute, 1033858465 );
};

};

#endif
