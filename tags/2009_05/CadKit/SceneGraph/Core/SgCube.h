
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgCube.h: Cube class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_CUBE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_CUBE_H_

#include "SgPrimitive.h"
#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
#endif

namespace CadKit
{
class SG_API SgCube : public SgPrimitive
{
public:

  SgCube();
  SgCube ( const SlVec3f &center, const SlFloat32 &size );
  SgCube ( const SgCube &cube );

  const SlVec3f &             getCenter() const { return _center; }
  void                        getCenter ( SlVec3f &center ) const { center = _center; }
  const SlFloat32 &           getSize() const { return _size; }

  void                        setCenter ( const SlVec3f &center ) { _center = center; }
  void                        setSize ( const SlFloat32 &size ) { _size = size; }

protected:

  SlVec3f _center;
  SlFloat32 _size;

  virtual ~SgCube();

  SG_DECLARE_DYNAMIC_NODE ( SgCube, 1047081875 );
};
};

#endif
