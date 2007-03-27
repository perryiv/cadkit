
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgScale.cpp: A Scale class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_SCALE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_SCALE_H_

#include "SgTransformation.h"

#include "Standard/SlVec3.h"


namespace CadKit
{
class SG_API SgScale : public SgTransformation
{
public:

  SgScale();
  SgScale ( const SlVec3f &s );
  SgScale ( const SgScale &scale );

  const SlVec3f &           getScale() const { return _scale; }

  void                      setScale ( const SlVec3f &scale ) { _scale.setValue ( scale ); }

protected:

  SlVec3f _scale;

  virtual ~SgScale();

  SG_DECLARE_DYNAMIC_NODE ( SgScale, 1033859030 );
};
};

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_SCALE_H_
