
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for moving a scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_MOVE_TOOL_FUNCTOR_H_
#define _CV_MOVE_TOOL_FUNCTOR_H_

#include "CadViewer/Functors/Tool.h"
#include "Usul/Functors/Interaction/Navigate/Direction.h"

#include "Usul/Math/Vector3.h"

namespace CV {
namespace Functors {


class MoveTool : public Tool
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( MoveTool );
  typedef Tool BaseClass;
  typedef Usul::Math::Vec3f Vector;
  typedef Usul::Functors::Interaction::Navigate::Direction Direction;

  MoveTool ( Unknown *unknown, 
             Direction *dir, 
             AnalogInput *ai, 
             float speed,
             const std::string& name );
  MoveTool ( const MoveTool &cb );
  virtual ~MoveTool();

  // Scale.
  virtual void          operator()();

private:

  float _speed;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_MOVE_TOOL_FUNCTOR_H_
