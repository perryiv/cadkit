
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that encapsulates the environment and model matrices.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_IO_MATRICES_CLASS_H_
#define _VRV_IO_MATRICES_CLASS_H_

#include "VRV/IO/CompileGuard.h"

#include "Usul/Math/Matrix44.h"


namespace VRV {
namespace IO {
namespace Detail {


template < class RealType_ > class Matrices
{
public:

  // Typedefs.
  typedef RealType_ RealType;
  typedef Usul::Math::Matrix44<RealType> Matrix;

  // Default construction.
  Matrices() : _env(), _model()
  {
    this->reset();
  }

  // Reset the members to their defaults.
  void reset()
  {
    _env.identity();
    _model.identity();
  }

  // Indicates that the start of a model has been encountered.
  void modelStart()
  {
    _model.identity();
  }

  // Indicates that the end of a model has been encountered.
  void modelFinish()
  {
    _model.identity();
  }

  // Return the product of the two matrices. Order is important. Since the 
  // environment matrix holds the unit-scale, it needs to be on the right 
  // so that the vertices are transformed first by this scale, and then by 
  // whatever is in the model-matrix, such as a rotation, translation, etc.
  Matrix product() const
  {
    return ( _model * _env );
  }

  // Get the model matrix.
  const Matrix &model() const
  {
    return _model;
  }

  // Set the model matrix.
  void model ( const Matrix &m )
  {
    _model = m;
  }

  // Get the environment matrix.
  const Matrix &env() const
  {
    return _env;
  }

  // Set the environment matrix.
  void env ( const Matrix &m )
  {
    _env = m;
  }

private:

  // No copy construction or assignment.
  Matrices ( const Matrices & );
  Matrices &operator = ( const Matrices & );

  Matrix _env;
  Matrix _model;
};


}; // namespace Detail
}; // namespace OSG
}; // namespace VRV


#endif // _VRV_IO_MATRICES_CLASS_H_
