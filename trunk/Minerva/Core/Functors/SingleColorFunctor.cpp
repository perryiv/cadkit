
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

#include "Usul/Interfaces/IPointLayer.h"
#include "Usul/Interfaces/ILineLayer.h"
#include "Usul/Interfaces/IPolygonLayer.h"
#include "Usul/Factory/RegisterCreator.h"

#include "osg/Geometry"
#include "osg/Array"

using namespace Minerva::Core::Functors;

SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( osg::Vec4 );
USUL_FACTORY_REGISTER_CREATOR ( SingleColorFunctor );

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

OsgTools::Legend::Icon * SingleColorFunctor::icon (  Usul::Interfaces::IUnknown *caller )
{
#if 0 // For now... always return quad for legend
  Usul::Interfaces::ILineLayer::QueryPtr lineLayer ( caller );
  if( lineLayer.valid() )
  {
    OsgTools::Legend::SolidColorLineIcon::RefPtr icon ( new OsgTools::Legend::SolidColorLineIcon );
    icon->geometryFunctor().lineWidth ( lineLayer->lineWidth() );
    icon->colorPolicy().color( this->color() );
    return icon.release();
  }

  Usul::Interfaces::IPolygonLayer::QueryPtr polygonLayer( caller );
  if( polygonLayer.valid() )
  {
    if ( polygonLayer->showBorder() )
    {
      OsgTools::Legend::SolidColorLineIcon::RefPtr icon ( new OsgTools::Legend::SolidColorLineIcon );
      icon->geometryFunctor().lineWidth ( polygonLayer->borderWidth() );
      icon->colorPolicy().color( this->color() );
      return icon.release();
    }
  }
#endif
  Usul::Interfaces::IPointLayer::QueryPtr pointLayer( caller );
  if( pointLayer.valid() )
  {
    typedef Usul::Interfaces::IPointLayer IPointLayer;

    switch ( pointLayer->primitiveID() )
    {
    case IPointLayer::SPHERE:
      {
        OsgTools::Legend::SolidMaterialSphereIcon::RefPtr icon ( new OsgTools::Legend::SolidMaterialSphereIcon );
        icon->colorPolicy().diffuse( this->color(), OsgTools::MaterialSetter::FRONT );
        icon->colorPolicy().diffuse( this->color(), OsgTools::MaterialSetter::BACK );
        return icon.release();
      }
    }
  }
  //#endif
  OsgTools::Legend::SolidColorQuadIcon::RefPtr icon ( new OsgTools::Legend::SolidColorQuadIcon );
  icon->colorPolicy().color ( this->color() );
  //icon->colorPolicy().diffuse( this->color(), OsgTools::MaterialSetter::FRONT );
  //icon->colorPolicy().diffuse( this->color(), OsgTools::MaterialSetter::BACK );

  return icon.release();
}
