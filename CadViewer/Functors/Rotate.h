
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Rotate functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_ROTATION_FUNCTORS_H_
#define _CV_ROTATION_FUNCTORS_H_

#include "CadViewer/Functors/Transform.h"


namespace CV {
namespace Functors {


class Rotate : public Transform
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Rotate );
  typedef Transform BaseClass;
  typedef BaseClass::Unknown Unknown;

  Rotate ( 
    Unknown *unknown,
    Direction *dir, 
    AnalogInput *ai, 
    float speed, // degrees / second
    unsigned int id );
  Rotate ( const Rotate &cb );

  // Rotate.
  virtual void          operator()();

  // Set the maximum degrees/second.
  void                  maxAngularSpeed ( float s );
  float                 maxAngularSpeed() const { return _speed; }

protected:

  virtual ~Rotate();

private:

  float _speed;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_ROTATION_FUNCTORS_H_
