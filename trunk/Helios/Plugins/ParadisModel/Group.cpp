
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Group.h"

#include "Usul/Adaptors/Random.h"
#include "Usul/Interfaces/IDecimateTriangles.h"
#include "Usul/Interfaces/ISmoothTriangles.h"
#include "Usul/Interfaces/ISubdivideTriangles.h"

#include "OsgTools/HasOption.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::Group( Vertices *vertices, Normals *normalsT, Normals *normalsV, Elements *indices ) :
	_vertices ( vertices ),
	_normalsT ( normalsT ),
	_normalsV ( normalsV ),
	_indices ( indices ),
	_group ( new osg::Group ),
	_mt ( new osg::MatrixTransform ),
	_geode ( new osg::Geode ),
	_geometry ( new osg::Geometry ),
	_material ( new osg::Material )
{
  this->_init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::Group() :
	_vertices ( new Vertices ),
	_normalsT ( new Normals ),
	_normalsV ( new Normals ),
	_indices ( new Elements ),
	_group ( new osg::Group ),
	_mt ( new osg::MatrixTransform ),
	_geode ( new osg::Geode ),
	_geometry ( new osg::Geometry ),
	_material ( new osg::Material )
{
  this->_init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::~Group()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Group::_init()
{
  _geometry->setVertexArray ( _vertices.get() );

  /*_geometry->setNormalArray ( _normalsT.get() );
  _geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );*/

  _geometry->setNormalArray ( _normalsV.get() );
  _geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );

  _geometry->addPrimitiveSet ( _indices.get() );

  _geode->addDrawable ( _geometry.get() );

  

#if 1
  Usul::Adaptors::Random< float > rd ( 0.0, 1.0 );
  float red ( rd() ), green ( rd() ), blue ( rd() );
#else

  osg::BoundingBox bb;
  for ( osg::Vec3Array::const_iterator iter = _vertices->begin(); iter != _vertices->end(); ++iter )
  {
    bb.expandBy ( *iter );
  }

  osg::Vec3 center ( bb.center() );

  //float red ( center.x() / ( _bb.xMax() - _bb.xMin() ) );
//  float green ( center.y() / ( _bb.yMax() - _bb.yMin() ) );
  //float blue ( center.z() / ( _bb.zMax() - _bb.zMin() ) );

  float red ( center.x() / 255.0f );
  float green ( center.y() / 255.0f );
  float blue ( center.z() / 255.0f );
#endif

  osg::ref_ptr< osg::StateSet > ss ( _group->getOrCreateStateSet() );

  _material->setDiffuse ( osg::Material::BACK,  osg::Vec4  ( 0.8f, 0.8f, 0.8f, 1.0f ) );
  _material->setDiffuse ( osg::Material::FRONT, osg::Vec4  ( red, green, blue, 1.0f ) );
  _material->setSpecular ( osg::Material::FRONT, osg::Vec4  ( 0.8f, 0.8f , 0.8f, 1.0f ) );
  _material->setShininess ( osg::Material::FRONT, 128 );

  ss->setAttribute ( _material.get(), osg::StateAttribute::ON );

  _group->addChild ( _mt.get() );
  _mt->addChild ( _geode.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Group::buildScene ( const Options &options, Unknown *caller )
{
  // Set the correct normal-vectors.
  if ( OsgTools::Options::has ( options, "normals", "per-vertex" ) )
  {
    _geometry->setNormalArray ( _normalsV.get() );
    _geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
  }
  else
  {
    _geometry->setNormalArray ( _normalsT.get() );
    _geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  }

  return _group.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visiblity flag.
//
///////////////////////////////////////////////////////////////////////////////

void Group::setVisibility ( bool b )
{
  if ( b )
  {
    _group->setNodeMask ( 0xffffffff );
  }
  else
  {
    _group->setNodeMask ( 0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the visiblity flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Group::getVisibility ( ) const
{
  return _group->getNodeMask (  ) != 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the translation.
//
///////////////////////////////////////////////////////////////////////////////

void Group::setTranslation ( const osg::Vec3f& translate )
{
  osg::Matrix m ( _mt->getMatrix() );
  m.setTrans ( translate );
  _mt->setMatrix ( m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the translation.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f Group::getTranslation ( ) const
{
  osg::Matrix m ( _mt->getMatrix() );
  return m.getTrans();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation.
//
///////////////////////////////////////////////////////////////////////////////

void Group::setRotation ( const osg::Quat& rotation )
{
  osg::Matrix m;
  m.setTrans ( _mt->getMatrix().getTrans() );
  m.makeRotate ( rotation );
  _mt->setMatrix ( m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the rotation.
//
///////////////////////////////////////////////////////////////////////////////

osg::Quat Group::getRotation ( ) const
{
  osg::Matrix m ( _mt->getMatrix() );
  osg::Quat q;
  m.get( q );
  return q;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the diffuse color.
//
///////////////////////////////////////////////////////////////////////////////

void Group::setDiffuseColor ( const osg::Vec4f& diffuse )
{
  _material->setDiffuse( osg::Material::FRONT_AND_BACK, diffuse );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the diffuse color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4f& Group::getDiffuseColor ( ) const
{
  return _material->getDiffuse( osg::Material::FRONT_AND_BACK );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the specular color.
//
///////////////////////////////////////////////////////////////////////////////

void Group::setSpecularColor ( const osg::Vec4f& specular )
{
  _material->setSpecular ( osg::Material::FRONT_AND_BACK, specular );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the specular color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4f& Group::getSpecularColor () const
{
  return _material->getSpecular ( osg::Material::FRONT_AND_BACK );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shininess value.
//
///////////////////////////////////////////////////////////////////////////////

void Group::setShininess( float s )
{
  _material->setShininess( osg::Material::FRONT_AND_BACK, s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shininess value.
//
///////////////////////////////////////////////////////////////////////////////

float Group::getShininess() const
{
  return _material->getShininess( osg::Material::FRONT_AND_BACK );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transparency value.
//
///////////////////////////////////////////////////////////////////////////////

void Group::setTransparency ( float t )
{
  _material->setTransparency ( osg::Material::FRONT_AND_BACK, t );

  osg::ref_ptr< osg::StateSet > ss ( _group->getOrCreateStateSet() );

  if ( t == 1.0 )
  {
    ss->setMode ( GL_BLEND,      osg::StateAttribute::OFF );
    ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON  );
    ss->setRenderingHint( osg::StateSet::DEFAULT_BIN );
  }
  else
  {
    ss->setMode ( GL_BLEND,      osg::StateAttribute::ON  );
    ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transparency value.
//
///////////////////////////////////////////////////////////////////////////////

float Group::getTransparency () const
{
  //float ta ( _material->getAmbient( osg::Material::FRONT_AND_BACK ).w() );
  float td ( _material->getDiffuse( osg::Material::FRONT_AND_BACK ).w() );
  //float ts ( _material->getSpecular( osg::Material::FRONT_AND_BACK ).w() );
  //float te ( _material->getEmissive( osg::Material::FRONT_AND_BACK ).w() );

  return td;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decimate the model.
//
///////////////////////////////////////////////////////////////////////////////

void Group::decimate ( Usul::Interfaces::IDecimateTriangles* decimate, float reduction )
{
  decimate->decimateTriangles( _vertices.get(), _indices.get(), _normalsT.get(), _normalsV.get(), reduction );

  _geometry->dirtyDisplayList();
  _geometry->dirtyBound();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the model.
//
///////////////////////////////////////////////////////////////////////////////

void Group::smooth ( Usul::Interfaces::ISmoothTriangles *smooth, unsigned int numIterations )
{
  smooth->smoothTriangles ( _vertices.get(), _indices.get(), _normalsT.get(), _normalsV.get(), numIterations );

  _geometry->dirtyDisplayList();
  _geometry->dirtyBound();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the model.
//
///////////////////////////////////////////////////////////////////////////////

void Group::subdivide ( Usul::Interfaces::ISubdivideTriangles *subdivide, unsigned int numIterations )
{
  subdivide->subdivideTriangles ( _vertices.get(), _indices.get(), _normalsT.get(), _normalsV.get(), numIterations );

  _geometry->dirtyDisplayList();
  _geometry->dirtyBound();
}
