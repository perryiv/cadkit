///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a torus.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_TORUS_H_
#define _OSG_TOOLS_TORUS_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include <osg/Vec4>


namespace OsgTools {


class OSG_TOOLS_EXPORT Torus
{
public:
  Torus():
    _inner_radius(1.0f), _outer_radius(2.0f),
    _start_sweep(osg::inDegrees(0.0f)), _end_sweep(osg::inDegrees(360.0f)),
    _circle_cuts(20), _sweep_cuts(20) {}

  Torus(float inner,float outer,
        float start_angle=0.0f,float end_angle=360.0f,
        unsigned int cross=20,unsigned int sweep=20):
    _inner_radius(inner), _outer_radius(outer),
    _start_sweep(start_angle), _end_sweep(end_angle),
    _circle_cuts(cross), _sweep_cuts(sweep) {}

  Torus(const Torus& torus) :
    _inner_radius(torus._inner_radius), _outer_radius(torus._outer_radius),
    _start_sweep(torus._start_sweep), _end_sweep(torus._end_sweep),
    _circle_cuts(torus._circle_cuts), _sweep_cuts(torus._sweep_cuts) {}

  inline bool valid() const
    {
      bool validity(false);
      if( (_outer_radius>_inner_radius) && (_outer_radius>0.0f) )
        validity = true;
      return( validity );
    }

  inline void set(float i,float o) {_inner_radius=i; _outer_radius=o;}

  osg::Geode* operator() () const;

  inline void setColor(const osg::Vec4& c) { _color = c; }
  inline const osg::Vec4& getColor() const { return _color; }

  inline void setInnerRadius(float inner) { _inner_radius = inner; }
  inline float getInnerRadius() const { return _inner_radius; }

  inline void setOuterRadius(float outer) { _outer_radius = outer; }
  inline float getOuterRadius() const { return _outer_radius; }

  inline void setStartSweep(float angle) { _start_sweep = angle; }
  inline float getStartSweep() const { return _start_sweep; }

  inline void setEndSweep(float angle) { _end_sweep = angle; }
  inline float getEndSweep() const { return _end_sweep; }

  inline void setCircleCuts(int cuts) { _circle_cuts = cuts; }
  inline int getCircleCuts() const { return _circle_cuts; }

  inline void setSweepCuts(int cuts) { _sweep_cuts = cuts; }
  inline int getSweepCuts() const { return _sweep_cuts; }

  virtual ~Torus() {}

private:
  float _inner_radius, _outer_radius;
  float _start_sweep, _end_sweep;
  unsigned int _circle_cuts, _sweep_cuts;
  osg::Vec4 _color;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_TORUS_H_
