
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgRotation.h: A Rotation class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_ROTATION_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_ROTATION_H_

#include "SgTransformation.h"

#include "Standard/SlVec3.h"


namespace CadKit
{
class SG_API SgRotation : public SgTransformation
{
public:

  SgRotation();
  SgRotation ( const SgRotation &rotation );
  SgRotation ( const SlVec3f &axes, const float &degrees );

  const SlVec3f &           getAxis() const { return _axis; }
  const float &             getDegrees() const { return _degrees; }

  void                      setAxis ( const SlVec3f &axis ) { _axis = axis; }
  void                      setDegrees ( const float &degrees ) { _degrees = degrees; }

protected:

  SlVec3f _axis;
  float _degrees;

  virtual ~SgRotation();

  SG_DECLARE_DYNAMIC_NODE ( SgRotation, 1033950761 );
};
};

#endif
