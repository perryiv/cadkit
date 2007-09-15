
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for seeking to the intersected portion of the scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_SEEK_FUNCTOR_H_
#define _CV_SEEK_FUNCTOR_H_

#include "CadViewer/Pick/Intersect.h"

#include "CadViewer/Interfaces/IRotationCenter.h"


namespace CV {
namespace Pick {


class Seek : public Intersect
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Seek );
  typedef Intersect BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef CV::Interfaces::IRotationCenterFloat RotationCenter;

  Seek ( 
    Unknown *unknown, 
    Direction *dir,
    const std::string& name );
  Seek ( const Seek &cb );

  // This actually does nothing.
  virtual void          operator()();

  // Seek to the intersected node, if any.
  void                  seek();

protected:

  virtual ~Seek();

  void                  _rotationCenter ( const osg::Vec3 &c );

private:

  RotationCenter::ValidQueryPtr _rc;
};


}; // namespace Pick
}; // namespace CV


#endif // _CV_SEEK_FUNCTOR_H_
