
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
