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
#include "Sphere.h"

#include "osg/Geode"
//#include <osg/PolygonMode>
//#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/Shape>

using namespace OsgTools;

//Sphere::Sphere():
  //ColorPolicyFunctor(),
//  _radius(1.0)
//{
//}

//Sphere::Sphere(float r)//:
  //ColorPolicyFunctor(), _radius(r)
//{
//}

//Sphere::Sphere(const osg::BoundingSphere& bs)//:
  //ColorPolicyFunctor(), _radius(bs.radius())
//{
//}

//Sphere::Sphere(const Sphere& s)//:
  //ColorPolicyFunctor(s), _radius(s._radius)
//{
//}

//Sphere& Sphere::operator = (const Sphere& s)
//{
//  ColorPolicyFunctor::operator = (s);
//  _radius = s._radius;
//}
//
//Sphere::~Sphere()
//{
//}

//osg::Node* Sphere<class ClassPolicy>::operator()()
//{
  //osg::ref_ptr<osg::TessellationHints> _hints;
  //osg::ref_ptr<osg::PolygonMode> _polygonmode;
  //_hints->setDetailRatio(0.5f);

  //osg::ref_ptr<osg::Material> _material = new osg::Material();
  //_material->setAmbient( osg::Material::FRONT, _color);
  //_material->setDiffuse( osg::Material::FRONT, _color);
  //_material->setSpecular( osg::Material::FRONT, osg::Vec4(0.5, 0.5, 0.5, 1.0) );
  //_material->setEmission( osg::Material::FRONT, _color);

  //osg::ref_ptr<osg::StateSet> _stateset = new osg::StateSet();
  //_stateset->setAttribute(_material.get());

  //_polygonmode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
  //_stateset->setAttribute(_polygonmode.get());

  //_shapedrawable->setTessellationHints(_hints.get());

  //osg::ref_ptr<osg::Sphere> _sphere = new osg::Sphere();
  //osg::ref_ptr<osg::ShapeDrawable> _shapedrawable = new osg::ShapeDrawable(_sphere.get());
  //_shapedrawable->setStateSet(_stateset.get());

//  osg::ref_ptr<osg::Geode> geode = new osg::Geode();
//  geode->addDrawable(_shapedrawable.get());
//  return geode.release();
//}
