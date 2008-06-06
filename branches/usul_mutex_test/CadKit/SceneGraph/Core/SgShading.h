
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgShading.h: Shading class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_SHADING_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_SHADING_H_

#include "SgAttribute.h"


namespace CadKit
{
class SG_API SgShading : public SgAttribute
{
public:

  enum Type
  {
    FLAT,
    SMOOTH
  };

  Type type;

  SgShading();

  virtual bool          isEqualVisibleProperties ( const SgNode &node ) const;

protected:

  virtual ~SgShading();

  SG_DECLARE_DYNAMIC_NODE ( SgShading, 1033858461 );
};
};

#endif
