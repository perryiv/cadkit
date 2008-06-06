
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgSphere.h: Sphere class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_SPHERE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_SPHERE_H_

#include "SgPrimitive.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
#endif

namespace CadKit
{
class SG_API SgSphere : public SgPrimitive
{
public:

  enum TessellationType
  {
    POLES,    // Default. 
    NO_POLES
  };

  SgSphere();
  SgSphere ( const SlVec3f &center, const SlFloat32 &radius );
  SgSphere ( const SgSphere &sphere );

  const SlVec3f &             getCenter() const { return _center; }
  void                        getCenter ( SlVec3f &center ) const { center = _center; }
  const SlUint32 &            getNumSlices() const { SL_ASSERT ( _tessellationType == POLES ); return _member0; }
  const SlUint32 &            getNumStacks() const { SL_ASSERT ( _tessellationType == POLES ); return _member1; }
  const SlUint32 &            getNumSubdivisions() const { SL_ASSERT ( _tessellationType == NO_POLES ); return _member1; }
  const SlFloat32 &           getRadius() const { return _radius; }
  const TessellationType &    getTessellationType() const { return _tessellationType; }

  void                        setCenter ( const SlVec3f &center ) { _center = center; }
  void                        setNumSlices ( const SlUint32 &numSlices );
  void                        setNumStacks ( const SlUint32 &numStacks );
  void                        setNumSubdivisions ( const SlUint32 &numSubdivisions ); // Reasonable range: [0,4].
  void                        setRadius ( const SlFloat32 &radius ) { _radius = radius; }
  void                        setTessellationType ( const TessellationType &type ) { _tessellationType = type; }

protected:

  virtual ~SgSphere();

  TessellationType _tessellationType;
  SlVec3f _center;
  SlFloat32 _radius;
  SlUint32 _member0; // These mean different things
  SlUint32 _member1; // depending on the tessellation.

  SG_DECLARE_DYNAMIC_NODE ( SgSphere, 1047081875 );
};
};

#endif
