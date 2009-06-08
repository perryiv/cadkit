
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Functors/SingleColorFunctor.h"

#include "Minerva/Interfaces/IPointLayer.h"
#include "Minerva/Interfaces/ILineLayer.h"
#include "Minerva/Interfaces/IPolygonLayer.h"

#include "OsgTools/Widgets/SolidIcon.h"

#include "Usul/Factory/RegisterCreator.h"

#include "osg/Geometry"
#include "osg/Array"

using namespace Minerva::Core::Functors;

USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_4 ( osg::Vec4d );
USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_4 ( osg::Vec4d );
SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( osg::Vec4d );
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

void SingleColorFunctor::color( const osg::Vec4d& color )
{
  _color = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4d& SingleColorFunctor::color () const
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

OsgTools::Widgets::Item * SingleColorFunctor::icon (  Usul::Interfaces::IUnknown *caller )
{
#if 0 // For now... always return quad for legend
  Minerva::Interfaces::ILineLayer::QueryPtr lineLayer ( caller );
  if( lineLayer.valid() )
  {
    OsgTools::Widgets::SolidColorLineIcon::RefPtr icon ( new OsgTools::Widgets::SolidColorLineIcon );
    icon->geometryFunctor().lineWidth ( lineLayer->lineWidth() );
    icon->colorPolicy().color( this->color() );
    return icon.release();
  }

  Minerva::Interfaces::IPolygonLayer::QueryPtr polygonLayer( caller );
  if( polygonLayer.valid() )
  {
    if ( polygonLayer->showBorder() )
    {
      OsgTools::Widgets::SolidColorLineIcon::RefPtr icon ( new OsgTools::Widgets::SolidColorLineIcon );
      icon->geometryFunctor().lineWidth ( polygonLayer->borderWidth() );
      icon->colorPolicy().color( this->color() );
      return icon.release();
    }
  }
#endif
  Minerva::Interfaces::IPointLayer::QueryPtr pointLayer( caller );
  if( pointLayer.valid() )
  {
    typedef Minerva::Interfaces::IPointLayer IPointLayer;

    switch ( pointLayer->primitiveID() )
    {
    case IPointLayer::SPHERE:
      {
        OsgTools::Widgets::SolidMaterialSphereIcon::RefPtr icon ( new OsgTools::Widgets::SolidMaterialSphereIcon );
        icon->colorPolicy().diffuse( this->color(), OsgTools::MaterialSetter::FRONT );
        icon->colorPolicy().diffuse( this->color(), OsgTools::MaterialSetter::BACK );
        return icon.release();
      }
    }
  }

  OsgTools::Widgets::SolidColorQuadIcon::RefPtr icon ( new OsgTools::Widgets::SolidColorQuadIcon );
  icon->colorPolicy().color ( this->color() );

  return icon.release();
}
