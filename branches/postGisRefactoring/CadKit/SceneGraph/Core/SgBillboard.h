
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgBillboard.h: A Billboard transformation class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_BILLBOARD_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_BILLBOARD_H_

#include "SgTransformation.h"


namespace CadKit
{
class SG_API SgBillboard : public SgTransformation
{
public:

  SgBillboard();
  SgBillboard ( const SgBillboard &billboard );

protected:

  virtual ~SgBillboard();

  SG_DECLARE_DYNAMIC_NODE ( SgBillboard, 1033950553 );
};
};

#endif
