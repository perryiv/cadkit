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

#include "Export.h"
//#include "ColorPolicyFunctor.h"

#include "osg/Geode"
#include "osg/ShapeDrawable"

namespace OsgTools
{

  template<class ColorPolicy>
  class Sphere// : public ColorPolicyFunctor
  {
  public:
    typedef ColorPolicy CP;
    Sphere(): _radius(0.5), _cp() {}
    Sphere(float r): _radius(r), _cp() {}
    Sphere(const Sphere& s): _radius(s._radius), _cp() {}
    virtual ~Sphere() {}

    Sphere& operator = (const Sphere& s) { _radius = s._radius; }

    virtual osg::Node* operator()()
    {
      osg::ref_ptr<osg::Sphere> s = new osg::Sphere(osg::Vec3(0.0,0.0,0.0),
                                                    _radius);
      osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable( s.get() );

      _cp( sd.get() );  // apply the color policy adjustments
      osg::ref_ptr<osg::Geode> node = new osg::Geode();
      node->addDrawable( sd.get() );
      return node.release();
    }

    void color_policy(const ColorPolicy& c) { _cp = c; }
    const ColorPolicy& color_policy() const { return _cp; }
    ColorPolicy& color_policy()             { return _cp; }

    void radius(float r) { _radius = r; }
    float radius() const { return _radius; }

  private:
    float _radius;
    ColorPolicy _cp;
  };

};

#endif
