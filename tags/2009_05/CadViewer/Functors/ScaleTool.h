
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for scaling a scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_SCALE_TOOL_FUNCTOR_H_
#define _CV_SCALE_TOOL_FUNCTOR_H_

#include "CadViewer/Functors/Tool.h"

#include "Usul/Math/Vector3.h"

namespace CV {
namespace Functors {


class ScaleTool : public Tool
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( ScaleTool );
  typedef Tool BaseClass;
  typedef Usul::Math::Vec3f Vector;

  ScaleTool ( Unknown *unknown, 
              Direction *dir, 
              AnalogInput *ai, 
              const Vector &scale, 
              float speed,
              const std::string& name );
  ScaleTool ( const ScaleTool &cb );
  virtual ~ScaleTool();

  // Scale.
  virtual void          operator()();

private:

  Vector _scale;
  float _speed;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_SCALE_TOOL_FUNCTOR_H_
