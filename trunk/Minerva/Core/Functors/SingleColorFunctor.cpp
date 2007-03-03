
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Functors/SingleColorFunctor.h"

#include "OsgTools/Legend/SolidIcon.h"

using namespace Minerva::Core::Functors;

SingleColorFunctor::SingleColorFunctor() : 
BaseClass(),
_color()
{
}


SingleColorFunctor::~SingleColorFunctor()
{
}

void SingleColorFunctor::color( const osg::Vec4& color )
{
  _color = color;
}

const osg::Vec4& SingleColorFunctor::color () const
{
  return _color;
}

osg::Vec4 SingleColorFunctor::operator() ( double value ) const
{
  return _color;
}

OsgTools::Legend::Icon * SingleColorFunctor::icon ()
{
  osg::ref_ptr< osg::Material > material ( new osg::Material );
  material->setDiffuse( osg::Material::FRONT_AND_BACK, this->color() );

  OsgTools::Legend::SolidIcon::RefPtr icon ( new OsgTools::Legend::SolidIcon );
  icon->material( material.get() );
  return icon.release();
}
