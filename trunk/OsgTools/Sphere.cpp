#include "osgJugglerPrecompiled.h"
#include <osgJuggler/highlightsphere.h>

#include <osg/PolygonMode>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/Shape>

using namespace osgJuggler;

// constructors
highlightsphere::highlightsphere() : osg::Geode(),  // default
				     _hints(new osg::TessellationHints),
				     _sphere(new osg::Sphere),
				     _material(new osg::Material),
				     _stateset(new osg::StateSet),
				     _polygonmode(new osg::PolygonMode),
				     _shapedrawable(new osg::ShapeDrawable)
{
  _color.set(0.0,0.0,0.0,1.0);
  _init();
}

highlightsphere::highlightsphere(const osg::Vec4& c) : osg::Geode(),  // color
						      _hints(new osg::TessellationHints),
						      _sphere(new osg::Sphere),
						      _material(new osg::Material),
						      _stateset(new osg::StateSet),
						      _polygonmode(new osg::PolygonMode),
						      _shapedrawable(new osg::ShapeDrawable)
{
  _color = c;
  _init();
}

highlightsphere::highlightsphere(const osg::BoundingSphere& bs) : osg::Geode(),  // BoundingSphere
								  _hints(new osg::TessellationHints),
								  _sphere(new osg::Sphere),
								  _material(new osg::Material),
								  _stateset(new osg::StateSet),
								  _polygonmode(new osg::PolygonMode),
								  _shapedrawable(new osg::ShapeDrawable)
{
  _color.set(0.0,0.0,0.0,1.0);
  float radius = bs.radius();
  float sub_radius = radius - 0.2*radius;
  _sphere->set(bs.center(),sub_radius);
  _init();
}

highlightsphere::highlightsphere(const osg::BoundingSphere& bs,
				 const osg::Vec4& c) : osg::Geode(),  // BS & color
						       _hints(new osg::TessellationHints),
						       _sphere(new osg::Sphere),
						       _material(new osg::Material),
						       _stateset(new osg::StateSet),
						       _polygonmode(new osg::PolygonMode),
						       _shapedrawable(new osg::ShapeDrawable)
{
  _color = c;
  float radius = bs.radius();
  float sub_radius = radius - 0.2*radius;
  _sphere->set(bs.center(),sub_radius);
  _init();
}

// destructor
highlightsphere::~highlightsphere()
{
}

// methods
void highlightsphere::_init()
{
  _hints->setDetailRatio(0.5f);

  _material->setAmbient( osg::Material::FRONT, _color);
  _material->setDiffuse( osg::Material::FRONT, _color);
  _material->setSpecular( osg::Material::FRONT, osg::Vec4(0.5, 0.5, 0.5, 1.0) );
  _material->setEmission( osg::Material::FRONT, _color);

  _stateset->setAttribute(_material.get());

  _polygonmode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
  _stateset->setAttribute(_polygonmode.get());

  _shapedrawable->setShape(_sphere.get());
  _shapedrawable->setTessellationHints(_hints.get());
  _shapedrawable->setStateSet(_stateset.get());

  addDrawable(_shapedrawable.get());
}

osg::Sphere* highlightsphere::getSphere()
{
  return(_sphere.get());
}

