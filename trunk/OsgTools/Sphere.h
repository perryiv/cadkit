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

#include "ColorPolicyFunctor.h"
#include "ColorSetter.h"
#include "MaterialSetter.h"

#include "osg/Geode"
#include "osg/ShapeDrawable"

namespace OsgTools
{

  template<class ColorPolicy>
  class Sphere : public ColorPolicyFunctor<ColorPolicy>
  {
  public:
    typedef ColorPolicyFunctor<ColorPolicy> CPF;
    Sphere(): CPF(), _radius(0.5) {}
    Sphere(float r): CPF(), _radius(r) {}
    Sphere(const Sphere& s): CPF(s), _radius(s._radius) {}
    virtual ~Sphere() {}

    Sphere& operator = (const Sphere& s)
    {
      CPF::operator =(s);
      _radius = s._radius;
    }

    virtual osg::Node* operator()() const
    {
      osg::ref_ptr<osg::Sphere> s = new osg::Sphere(osg::Vec3(0.0,0.0,0.0),_radius);
      osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable( s.get() );

      _cp( sd.get() );  // apply the color policy adjustments
      osg::ref_ptr<osg::Geode> node = new osg::Geode();
      node->addDrawable( sd.get() );
      return node.release();
    }

    void radius(float r) { _radius = r; }
    float radius() const { return _radius; }

  private:
    float _radius;
  };

  typedef Sphere<ColorSetter> ColorSphere;
  typedef Sphere<MaterialSetter> MaterialSphere;
};

#endif
