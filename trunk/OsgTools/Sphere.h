
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
#include "osg/BoundingSphere"
#include "osg/ShapeDrawable"

namespace OsgTools
{
  template<class ColorPolicy>
  class Sphere : public ColorPolicyFunctor<ColorPolicy>
  {
  public:
    typedef ColorPolicyFunctor<ColorPolicy> CPF;

    Sphere(): CPF(), 
      _radius      ( 0.5f ),
      _detailRatio ( 0.75f )
    {
    }

    Sphere ( float r, float dr = 0.75f ) : CPF(),
      _radius      ( r ),
      _detailRatio ( dr )
    {
    }

    Sphere ( const Sphere& s ) : CPF ( s ),
      _radius      ( s._radius ),
      _detailRatio ( s._detailRatio )
    {
    }

    Sphere ( const osg::BoundingSphere &s ) : CPF(),
      _radius      ( s.radius() ),
      _detailRatio ( 0.5f )
    {
    }

    virtual ~Sphere()
    {
    }

    Sphere& operator = (const Sphere& s)
    {
      CPF::operator =(s);
      _radius = s._radius;
      _detailRatio = s._detailRatio;
    }

    virtual osg::Node* operator()() const
    {
      osg::ref_ptr<osg::Sphere> s = new osg::Sphere(osg::Vec3(0.0,0.0,0.0),_radius);
      osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable( s.get() );
      osg::ref_ptr<osg::TessellationHints> hints ( new osg::TessellationHints );
      hints->setDetailRatio ( _detailRatio );
      sd->setTessellationHints ( hints.get() );

      // Needed to make gcc 4.1.1 happy
      //this->color_policy()( sd.get() );

      osg::ref_ptr<osg::Geode> node = new osg::Geode();
      node->setName("OsgTools_Sphere_geode");
      node->addDrawable( sd.get() );
      return node.release();
    }

    void radius(float r) { _radius = r; }
    float radius() const { return _radius; }

  private:
    float _radius;
    float _detailRatio;
  };

  typedef Sphere<ColorSetter> ColorSphere;
  typedef Sphere<MaterialSetter> MaterialSphere;
};

#endif
