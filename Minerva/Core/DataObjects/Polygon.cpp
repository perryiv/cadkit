
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to represent boundary data.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/DataObjects/Polygon.h"
#include "Minerva/Core/DataObjects/UserData.h"

#include "Usul/Interfaces/IPolygonData.h"

#include "osg/Material"
#include "osg/PolygonOffset"
#include "osg/Group"

using namespace Minerva::Core::DataObjects;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Polygon::Polygon() :
BaseClass(),
_group ( new osg::Group ),
_showBorder( false ),
_showInterior ( true )
{
  _group->setUserData ( new UserData ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Polygon::~Polygon()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Polygon::buildScene( )
{
  if( this->dirty() )
  {
    _group->removeChild( 0, _group->getNumChildren() );

    if( this->showInterior() )
    {
      _group->addChild( this->_buildPolygons() );
    }

    if( this->showBorder() )
    {
      _group->addChild( BaseClass::buildScene() );
    }
  }

  return _group.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a mesh.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Polygon::_buildPolygons( )
{
  Usul::Interfaces::IPolygonData::QueryPtr polygon ( this->geometry() );

  if( polygon.valid() )
  {
    osg::ref_ptr < osg::Node > node ( polygon->buildPolygonData() );

    osg::Vec4 color ( this->color() );
    osg::ref_ptr < osg::Material > mat ( new osg::Material );
    mat->setDiffuse ( osg::Material::FRONT_AND_BACK, color );

    osg::ref_ptr < osg::StateSet > ss ( node->getOrCreateStateSet () );

    // Set the material.
    ss->setAttribute ( mat.get(), osg::StateAttribute::ON );

    // Set state set modes depending on alpha value.
    if( 1.0f == color.w() )
    {
      ss->setMode ( GL_BLEND,      osg::StateAttribute::OFF );
      ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON  );
      
      ss->setRenderBinDetails( this->renderBin(), "RenderBin" );
    }
    else
    {
      ss->setMode ( GL_BLEND,      osg::StateAttribute::ON  | osg::StateAttribute::OVERRIDE );
      ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
      ss->setRenderBinDetails( this->renderBin(), "RenderBin" );
    }

    osg::ref_ptr< osg::PolygonOffset > offset ( new osg::PolygonOffset( -1.0f, -1.0f ) );
    ss->setMode ( GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
    ss->setAttribute( offset.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

    return node.release();
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the draw border flag.
//
///////////////////////////////////////////////////////////////////////////////

void Polygon::showBorder( bool b )
{
  _showBorder = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw border flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Polygon::showBorder() const
{
  return _showBorder;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the draw interior flag.
//
///////////////////////////////////////////////////////////////////////////////

void Polygon::showInterior( bool b )
{
  _showInterior = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw interior flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Polygon::showInterior() const
{
  return _showInterior;
}

