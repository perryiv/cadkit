
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgTransform.h: A Transformation class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_TRANSFORM_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_TRANSFORM_H_

#include "SgTransformation.h"

#include "Standard/SlMatrix4.h"


namespace CadKit
{
class SG_API SgTransform : public SgTransformation
{
public:

  SgTransform();
  SgTransform ( const SlMatrix4f &matrix );
  SgTransform ( const SgTransform &transform );

  const SlMatrix4f &    getMatrix() const { return _matrix; }
  void                  setMatrix ( const SlMatrix4f &matrix ) { _matrix.setValue ( matrix ); }

protected:

  SlMatrix4f _matrix;

  virtual ~SgTransform();

  SG_DECLARE_DYNAMIC_NODE ( SgTransform, 1033950241 );
};

};

#endif
