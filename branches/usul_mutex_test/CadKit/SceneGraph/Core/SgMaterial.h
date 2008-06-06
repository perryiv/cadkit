
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgMaterial.h: Material class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_MATERIAL_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_MATERIAL_H_

#include "SgAttribute.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlMaterial.h"
#endif


namespace CadKit
{
class SG_API SgMaterial : public SgAttribute
{
public:

  enum Side
  {
    FRONT,
    BACK,
    FRONT_AND_BACK
  };

  Side side;
  SlMaterialf material; // All fields range from [0,1].

  SgMaterial();
  SgMaterial ( const SgMaterial &material );

  virtual bool          isEqualVisibleProperties ( const SgNode &node ) const;

  void                  setValue ( const SgMaterial &material );

protected:

  virtual ~SgMaterial();

  SG_DECLARE_DYNAMIC_NODE ( SgMaterial, 1033858460 );
};

};

#endif
