
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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

  SG_DECLARE_DYNAMIC_NODE(SgSphere,0x0000102D);
};
};

#endif
