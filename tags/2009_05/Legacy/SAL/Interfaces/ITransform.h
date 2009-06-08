
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with a transformation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_TRANSFORM_H_
#define _SAL_INTERFACE_TRANSFORM_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Matrix44.h"


namespace SAL {
namespace Interfaces {

struct INode;

template
<
  unsigned long iid,
  class RealType
>
struct ITransform : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITransform );
  typedef Usul::Math::Matrix44<RealType> Matrix;

  /// Id for this interface.
  enum { IID = iid };

  /// Get/set the transformation matrix.
  virtual void    getTransformMatrix ( Matrix & ) const = 0;
  virtual void    setTransformMatrix ( const Matrix & ) = 0;
};


typedef ITransform<1313865513u,float>  ITransformFloat;
typedef ITransform<2311315484u,double> ITransformDouble;


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_TRANSFORM_H_
