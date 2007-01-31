
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting and setting the center of rotation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_INTERFACE_ROTATION_CENTER_H_
#define _CV_INTERFACE_ROTATION_CENTER_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"


namespace CV {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct IRotationCenter : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRotationCenter );
  typedef Usul::Math::Vector3 <RealType> Vector;

  /// Id for this interface.
  enum { IID = iid };

  // Get/set the wand's offset.
  virtual void              rotationCenter ( Vector &v ) const = 0;
  virtual void              rotationCenter ( const Vector &v ) = 0;
};


typedef IRotationCenter<1626193591u,float>  IRotationCenterFloat;
typedef IRotationCenter<2460453746u,double> IRotationCenterDouble;


}; // namespace Interfaces
}; // namespace CV


#endif // _CV_INTERFACE_WAND_STATE_H_
