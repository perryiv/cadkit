
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Translation functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_TRANSLATION_FUNCTORS_H_
#define _CV_TRANSLATION_FUNCTORS_H_

#include "CadViewer/Functors/Transform.h"


namespace CV {
namespace Functors {


class Translate : public Transform
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Translate );
  typedef Transform BaseClass;
  typedef BaseClass::Unknown Unknown;

  Translate (
    Unknown *unknown, 
    Direction *dir, 
    AnalogInput *ai,
    float speed, // percentage-of-world-radius / seconds
    unsigned int id );
  Translate ( const Translate &cb );

  // Translate.
  virtual void          operator()();

  // Set/get the maximum relative speed. The units are 
  // percentage-of-world-radius / seconds.
  void                  maxRelativeSpeed ( float s );
  float                 maxRelativeSpeed() const { return _speed; }

protected:

  virtual ~Translate();

private:

  float _speed;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_TRANSLATION_FUNCTORS_H_
