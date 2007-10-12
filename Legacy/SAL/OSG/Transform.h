
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

#ifndef _OSG_SAL_TRANSFORM_CLASS_H_
#define _OSG_SAL_TRANSFORM_CLASS_H_

#include "SAL/OSG/Group.h"
#include "SAL/Interfaces/ITransform.h"

namespace osg { class MatrixTransform; };


namespace SAL {
namespace OSG {


class Transform : public SAL::OSG::Group,
                  public SAL::Interfaces::ITransformFloat,
                  public SAL::Interfaces::ITransformDouble
{
public:

  /// Typedefs.
  typedef SAL::OSG::Group BaseClass;
  typedef osg::MatrixTransform MT;
  typedef SAL::Interfaces::ITransformFloat::Matrix Matrixf;
  typedef SAL::Interfaces::ITransformDouble::Matrix Matrixd;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Transform );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Transform ( MT * );

protected:

  typedef USUL_VALID_REF_POINTER ( MT ) ValidOsgMtPtr;

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

  ValidOsgMtPtr _mt;
};


}; // namespace OSG
}; // namespace SAL


#endif // _OSG_SAL_TRANSFORM_CLASS_H_
