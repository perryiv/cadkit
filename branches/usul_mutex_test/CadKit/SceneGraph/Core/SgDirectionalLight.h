
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgDirectionalLight.h: Directional light class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_DIRECTIONAL_LIGHT_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_DIRECTIONAL_LIGHT_H_

#include "SgLight.h"

#include "Standard/SlVec3.h"


namespace CadKit
{
class SG_API SgDirectionalLight : public SgLight
{
public:

  SgDirectionalLight();
  SgDirectionalLight ( const SgDirectionalLight &light );

  const SlVec3f &     getDirection() const { return _direction; }

  void                setDirection ( const SlVec3f &direction ) { _direction.setValue ( direction ); }
  void                setValue ( const SgDirectionalLight &light );

protected:

  SlVec3f _direction;

  virtual ~SgDirectionalLight();

  SG_DECLARE_DYNAMIC_NODE ( SgDirectionalLight, 1033953000 );
};

};

#endif
