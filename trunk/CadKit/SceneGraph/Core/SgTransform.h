
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

#include "Standard/SlMatrix44.h"


namespace CadKit
{
class SG_API SgTransform : public SgTransformation
{
public:

  SgTransform();
  SgTransform ( const SlMatrix44f &matrix );
  SgTransform ( const SgTransform &transform );

  const SlMatrix44f &    getMatrix() const { return _matrix; }
  void                  setMatrix ( const SlMatrix44f &matrix ) { _matrix.setValue ( matrix ); }

protected:

  SlMatrix44f _matrix;

  virtual ~SgTransform();

  SG_DECLARE_DYNAMIC_NODE ( SgTransform, 1033950241 );
};

};

#endif
