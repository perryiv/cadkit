
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a ray.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Ray.h"

#include "Usul/Errors/Checker.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/LineWidth"

using namespace OsgTools;

typedef Usul::Errors::Checker ErrorChecker;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Ray::Ray() :
  _color ( 1, 0, 0, 1 ),
  _start ( 0, 0, 0 ),
  _end   ( 0, 0, 1 ),
  _thickness ( 1 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the ray.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Ray::operator()() const
{
  ErrorChecker ( 1069023821, _thickness > 0 );

  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
  osg::ref_ptr<osg::Vec3Array> p ( new osg::Vec3Array );

  p->resize ( 2 );
  (*p)[0] = _start;
  (*p)[1] = _end;

  geom->setVertexArray ( p.get() );
  geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, 2 ) );

  osg::ref_ptr<osg::Vec4Array> c ( new osg::Vec4Array );
  c->resize ( 1 );
  (*c)[0] = _color;

  geom->setColorArray ( c.get() );
  geom->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE_SET );

  osg::ref_ptr<osg::LineWidth> lw ( new osg::LineWidth );
  lw->setWidth ( _thickness );

  osg::ref_ptr<osg::StateSet> state = geom->getOrCreateStateSet();
  state->setAttribute ( lw.get() );
  state->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );

  geode->addDrawable ( geom.get() );

  return geode.release();
}
