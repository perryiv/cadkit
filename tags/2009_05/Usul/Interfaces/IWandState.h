
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting and setting the wand's state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_WAND_STATE_H_
#define _USUL_INTERFACE_WAND_STATE_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Vector3.h"


namespace Usul {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct IWandState : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWandState );
  typedef Usul::Math::Matrix44<RealType> Matrix;
  typedef Usul::Math::Vector3 <RealType> Vector;

  /// Id for this interface.
  enum { IID = iid };

  // Get the wand's position.
  virtual void              wandPosition ( Vector &p ) const = 0;

  // Get the wand's rotation matrix.
  virtual void              wandRotation ( Matrix &R ) const = 0;

  // Get the wand's matrix.
  virtual void              wandMatrix ( Matrix &M ) const = 0;

  // Get/set the wand's offset.
  virtual void              wandOffset ( Vector &v ) const = 0;
  virtual void              wandOffset ( const Vector &v ) = 0;
};


typedef IWandState<1076052184,float>  IWandStateFloat;
typedef IWandState<1076052185,double> IWandStateDouble;


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_WAND_STATE_H_
