
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
//  SgEllipse:
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_ELLIPSE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_ELLIPSE_H_

#include "SgPrimitive.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
#endif


namespace CadKit
{
class SG_API SgEllipse : public SgPrimitive
{
public:

  SgEllipse();
  SgEllipse ( const SlVec3f &center, const SlFloat32 &majorRadius, const SlFloat32 &minorRadius, const SlInt32 &numVerts );

  void                getCenter ( SlVec3f &center ) const { center.setValue ( _center ); }
  const SlVec3f &     getCenter() const { return _center; }
  const SlFloat32 &   getMajorRadius() const { return _majorRadius; }
  const SlFloat32 &   getMinorRadius() const { return _minorRadius; }
  const SlInt32 &     getNumVertices() const { return _numVerts; }

  bool                makeVertices();

  void                setCenter ( const SlVec3f &center );
  void                setCenter ( const SlFloat32 &x, const SlFloat32 &y, const SlFloat32 &z );
  void                setMajorRadius ( const SlFloat32 &majorRadius );
  void                setMinorRadius ( const SlFloat32 &minorRadius );
  void                setNumVertices ( const SlInt32 &numVerts );
  void                setRadii ( const SlFloat32 &majorRadius, const SlFloat32 &minorRadius );

protected:

  SlVec3f _center;
  SlFloat32 _majorRadius;
  SlFloat32 _minorRadius;
  SlInt32 _numVerts;
  bool _needToMakeNewVertices;
  SlVec3f *_vertex;

  virtual ~SgEllipse();

  SG_DECLARE_DYNAMIC_NODE(SgEllipse,0x00001011);
};
};

#endif
