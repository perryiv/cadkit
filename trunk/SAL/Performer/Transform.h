
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Transform class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PERFORMER_SAL_TRANSFORM_CLASS_H_
#define _PERFORMER_SAL_TRANSFORM_CLASS_H_

#include "SAL/Performer/Group.h"
#include "SAL/Interfaces/ITransform.h"

class pfDCS;


namespace SAL {
namespace Performer {


class Transform : public SAL::Performer::Group,
                  public SAL::Interfaces::ITransformFloat,
                  public SAL::Interfaces::ITransformDouble
{
public:

  /// Typedefs.
  typedef SAL::Performer::Group BaseClass;
  typedef SAL::Interfaces::ITransformFloat::Matrix Matrixf;
  typedef SAL::Interfaces::ITransformDouble::Matrix Matrixd;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Transform );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Transform ( ::pfDCS * );

protected:

  typedef USUL_VALID_REF_POINTER ( ::pfDCS ) ValidPfDcsPtr;

  /// No default construction.
  Transform();

  /// Do not copy.
  Transform ( const Transform & );
  Transform &operator = ( const Transform & );

  /// Use reference counting.
  virtual ~Transform();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::ITransformFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Get/set the transformation matrix.
  virtual void        getTransformMatrix ( Matrixf & ) const;
  virtual void        setTransformMatrix ( const Matrixf & );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::ITransformDouble
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Get/set the transformation matrix.
  virtual void        getTransformMatrix ( Matrixd & ) const;
  virtual void        setTransformMatrix ( const Matrixd & );

private:

  ValidPfDcsPtr _dcs;
};


}; // namespace Performer
}; // namespace SAL


#endif // _PERFORMER_SAL_TRANSFORM_CLASS_H_
