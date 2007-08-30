
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_ANIMATE_FRAME_H__
#define __VRV_ANIMATE_FRAME_H__

#include "VRV/Export.h"

#include "Usul/Base/Object.h"

#include "Serialize/XML/Macros.h"

#include "osg/Matrix"
#include "osg/Quat"
#include "osg/Vec3"

namespace VRV {
namespace Animate {

class VRV_EXPORT Frame : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_REF_POINTERS ( Frame );

  /// Construction.
  Frame ();
  Frame ( const osg::Matrix& m );

  /// Get/Set the matrix.
  void                     matrix ( const osg::Matrix& m );
  const osg::Matrix&       matrix () const;

  /// Get the rotation.
  osg::Quat                rotation () const;

  /// Get the translation.
  osg::Vec3                translation () const;

protected:
  virtual ~Frame ();

private:
  osg::Matrix _m;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( Frame );
};

}
}


#endif // __VRV_ANIMATE_FRAME_H__
