
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

#include "Serialize/XML/RegisterCreator.h"

using namespace Minerva::Core::Functors;

SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( osg::Vec4 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SingleColorFunctor::SingleColorFunctor() : 
BaseClass(),
_color()
{
  SERIALIZE_XML_ADD_MEMBER ( _color );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SingleColorFunctor::~SingleColorFunctor()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

BaseColorFunctor* SingleColorFunctor::clone() const
{
  SingleColorFunctor::RefPtr copy ( new SingleColorFunctor );
  copy->_color = this->_color;

  return copy.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void SingleColorFunctor::color( const osg::Vec4& color )
{
  _color = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4& SingleColorFunctor::color () const
{
  return _color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4 SingleColorFunctor::operator() ( double value ) const
{
  return _color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the icon.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Legend::Icon * SingleColorFunctor::icon ()
{
  osg::ref_ptr< osg::Material > material ( new osg::Material );
  material->setDiffuse( osg::Material::FRONT_AND_BACK, this->color() );

  OsgTools::Legend::SolidIcon::RefPtr icon ( new OsgTools::Legend::SolidIcon );
  icon->material( material.get() );
  return icon.release();
}