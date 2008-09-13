
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to update the matrix in a thread safe manner.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ___MINERVA_CORE_OSG_MATRIX_TRANSFORM_H__
#define ___MINERVA_CORE_OSG_MATRIX_TRANSFORM_H__

#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"

#include "osg/MatrixTransform"

namespace Minerva {
namespace Core {
namespace OSG {

  
class MatrixTransform : public osg::MatrixTransform
{
public:
  
  typedef osg::MatrixTransform BaseClass;
  typedef osg::ref_ptr<MatrixTransform> RefPtr;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  
  MatrixTransform();

  // Set the matrix.
  void           matrix ( const osg::Matrixd& m );
  
  // Traverse.
  virtual void   traverse ( osg::NodeVisitor& );
  
protected:
  
  virtual ~MatrixTransform();
  
  bool _isDirty() const;
  
private:
  
  mutable Mutex _mutex;
  osg::Matrixd _matrix;
  bool _dirty;
};


}
}
}

#endif // ___MINERVA_CORE_OSG_MATRIX_TRANSFORM_H__
