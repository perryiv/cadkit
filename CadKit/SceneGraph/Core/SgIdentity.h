
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgIdentity.h: Loads the identity matrix onto the current matrix stack.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_IDENTITY_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_IDENTITY_H_

#include "SgTransformation.h"


namespace CadKit
{
class SG_API SgIdentity : public SgTransformation
{
public:

  SgIdentity();
  SgIdentity ( const SgIdentity &identity );

protected:

  virtual ~SgIdentity();

  SG_DECLARE_DYNAMIC_NODE ( SgIdentity, 1033950658 );
};
};

#endif
