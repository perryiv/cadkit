///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a Sphere.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _osg_tools_builder_sphere_
#define _osg_tools_builder_sphere_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include "osg/Vec4"
#include "osg/BoundingSphere"

// forward declarations

namespace OsgTools
{

  class OSG_TOOLS_EXPORT Sphere
  {
  public:
    Sphere(float r=1.0,
           const osg::Vec4& c=osg::Vec4(1.0,0.0,0.0,1.0) );
    Sphere(const osg::BoundingSphere&,
           const osg::Vec4& c=osg::Vec4(1.0,0.0,0.0,1.0));
    Sphere(const Sphere&);
    Sphere& operator = (const Sphere&);
    virtual ~Sphere();

    osg::Geode* operator()();

    void radius(float r) { _radius = r; }
    float radius() const { return _radius; }

    void color(const osg::Vec4& c) { _color = c; }
    const osg::Vec4& color() const { return _color; }

  private:
    float _radius;
    osg::Vec4 _color;
  };

};
    
#endif
