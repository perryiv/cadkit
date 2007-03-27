
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread for reading a model class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_READ_MODEL_CLASS_H_
#define _VIRTUAL_REALITY_VIEWER_READ_MODEL_CLASS_H_

#include "VRV/Core/Thread.h"

#include "Usul/Math/Matrix44.h"


namespace VRV {
namespace Threads {


class ReadModel : public VRV::Threads::Thread
{
public:

  // Typedefs.
  typedef VRV::Threads::Thread BaseClass;
  typedef Usul::Math::Matrix44f Matrix;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Unknown::ValidQueryPtr ValidUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ReadModel );

  ReadModel ( const std::string &filename, const Matrix &matrix, Unknown *caller );

protected:

  virtual ~ReadModel();

  virtual void            _run();

private:

  std::string _filename;
  Matrix _matrix;
  Unknown::ValidRefPtr _caller;
};


}; // namespace Threads
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_READ_MODEL_CLASS_H_
