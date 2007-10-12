
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Light interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _JSAL_INTERFACE_LIGHT_H_
#define _JSAL_INTERFACE_LIGHT_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector3.h"


namespace JSAL {
namespace Interfaces {


struct ILight : public Usul::Interfaces::IUnknown
{
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec4f Vec4f;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILight );

  /// Id for this interface.
  enum { IID = 1079419079u };

  /// Light states.
  enum State { ON, OFF };

  // Set/get the lighting properties.
  virtual void          getAmbientLightColor ( Vec4f &c ) const = 0;
  virtual void          setAmbientLightColor ( const Vec4f &c ) = 0;
  virtual void          getDiffuseLightColor ( Vec4f &c ) const = 0;
  virtual void          setDiffuseLightColor ( const Vec4f &c ) = 0;
  virtual void          getSpecularLightColor ( Vec4f &c ) const = 0;
  virtual void          setSpecularLightColor ( const Vec4f &c ) = 0;
  virtual void          getLightDirection ( Vec3f &d ) const = 0;
  virtual void          setLightDirection ( const Vec3f &d ) = 0;
  virtual State         getLightState() const = 0;
  virtual void          setLightState ( State ) = 0;
};


}; // namespace Interfaces
}; // namespace JSAL


#endif // _JSAL_INTERFACE_LIGHT_H_
