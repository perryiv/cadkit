
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgLight.h: Abstract light class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_LIGHT_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_LIGHT_H_

#include "SgNode.h"

#include "Standard/SlColor.h"


namespace CadKit
{
class SG_API SgLight : public SgNode
{
public:

  const SlColorf &        getAmbient()   const { return _ambient; }
  const SlColorf &        getDiffuse()   const { return _diffuse; }
  const SlColorf &        getSpecular()  const { return _specular; }

  // Turn the light on/off.
  void                    on()  { this->addNodeFlags    ( ACCEPT_VISITORS ); }
  void                    off() { this->removeNodeFlags ( ACCEPT_VISITORS ); }

  void                    setAmbient   ( const SlVec4f &ambient )   { _ambient.setColor  ( ambient ); }
  void                    setDiffuse   ( const SlVec4f &diffuse )   { _diffuse.setColor  ( diffuse ); }
  void                    setSpecular  ( const SlVec4f &specular )  { _specular.setColor ( specular ); }
  void                    setValue     ( const SgLight &light );

protected:

  SlColorf _ambient;
  SlColorf _diffuse;
  SlColorf _specular;

  SgLight();
  SgLight ( const SgLight &light );
  virtual ~SgLight();

  SG_DECLARE_NODE ( SgLight, 1033952946 );
};

};

#endif
