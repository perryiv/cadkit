///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to set the material of osg types.
//
///////////////////////////////////////////////////////////////////////////////

#include "MaterialSetter.h"

#include "osg/Material"
#include "osg/Node"
#include "osg/Drawable"

using namespace OsgTools;

MaterialSetter::MaterialSetter():
  _af ( 0.2f,0.2f,0.2f,1.0f ),
  _df ( 0.8f,0.8f,0.8f,1.0f ),
  _ef ( 0.0,0.0f,0.0f,1.0f ),
  _spf ( 0.0f,0.0f,0.0f,1.0f ),
  _ab ( 0.2f,0.2f,0.2f,1.0f),
  _db ( 0.8f,0.8f,0.8f,1.0f),
  _eb ( 0.0f,0.0f,0.0f,1.0f),
  _spb ( 0.0f,0.0f,0.0f,1.0f),
  _shb ( 0.5f),
  _shf ( 0.5f)
{
}

MaterialSetter::MaterialSetter(const MaterialSetter& ms):
_af(ms._af), _df(ms._df), _ef(ms._ef), _spf(ms._spf),
_ab(ms._ab), _db(ms._db), _eb(ms._eb), _spb(ms._spb),
_shb(ms._shf), _shf(ms._shb)
{
}

MaterialSetter::~MaterialSetter()
{
}

MaterialSetter& MaterialSetter::operator =(const MaterialSetter& ms)
{
  _af=ms._af; _df=ms._df; _ef=ms._ef; _shf=ms._shf; _spf=ms._spf;
  _ab=ms._ab; _db=ms._db; _eb=ms._eb; _shb=ms._shb; _spb=ms._spb;
  return (*this);
}

void MaterialSetter::operator ()(osg::Node* n) const
{
  adjust_stateset( n->getOrCreateStateSet() );
}

void MaterialSetter::operator ()(osg::Drawable* d) const
{
  adjust_stateset( d->getOrCreateStateSet() );
}

void MaterialSetter::adjust_stateset(osg::StateSet* ss) const
{
  osg::ref_ptr<osg::Material> mat = new osg::Material();

  // front face properties
  mat->setAmbient(osg::Material::FRONT,_af);
  mat->setDiffuse(osg::Material::FRONT_AND_BACK,_df);
  mat->setEmission(osg::Material::FRONT,_ef);
  mat->setShininess(osg::Material::FRONT,_shf);
  mat->setSpecular(osg::Material::FRONT,_spf);

  // back face properties
  mat->setAmbient(osg::Material::BACK,_ab);
  mat->setDiffuse(osg::Material::BACK,_db);
  mat->setEmission(osg::Material::BACK,_eb);
  mat->setShininess(osg::Material::BACK,_shb);
  mat->setSpecular(osg::Material::BACK,_spb);

  ss->setAttribute(mat.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
}

const osg::Vec4& MaterialSetter::ambient(Face face)
{
  if( face == BACK )
    return _ab;

  else
    return _af;
}

const osg::Vec4& MaterialSetter::diffuse(Face face)
{
  if( face == BACK )
    return _db;

  else
    return _df;
}

const osg::Vec4& MaterialSetter::emissive(Face face)
{
  if( face == BACK )
    return _eb;

  else
    return _ef;
}

float MaterialSetter::shininess(Face face)
{
  if( face == BACK )
    return _shb;

  else
    return _shf;
}

const osg::Vec4& MaterialSetter::specular(Face face)
{
  if( face == BACK )
    return _spb;

  else
    return _spf;
}
