
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

#include "osg/Material"
#include "osg/PolygonOffset"
#include "osg/Depth"
#include "osg/Group"

using namespace Minerva::Core::DataObjects;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Boundary::Boundary() :
BaseClass(),
_group ( new osg::Group )
{
  _group->setUserData ( new UserData ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Boundary::~Boundary()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a mesh.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Boundary::buildScene( )
{
  if( this->dirty() )
  {
    osg::Vec4 color ( this->color() );

    _group->removeChild( 0, _group->getNumChildren() );

    _group->addChild( this->geometry()->buildScene() );

    osg::ref_ptr < osg::Material > mat ( new osg::Material );
    mat->setDiffuse ( osg::Material::FRONT_AND_BACK, color );

    osg::ref_ptr < osg::StateSet > ss ( _group->getOrCreateStateSet () );

    //if( 1.0f == color.w() )
    //{
    //  ss->setMode ( GL_BLEND,      osg::StateAttribute::OFF );
    //  ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON  );
    //  
    //  ss->setRenderBinDetails( this->renderBin(), "RenderBin" );
    //}
    //else
    //{
    //  //mat->setTransparency( osg::Material::FRONT_AND_BACK, color.w() );
    //  ss->setMode ( GL_BLEND,      osg::StateAttribute::ON  | osg::StateAttribute::OVERRIDE );
    //  ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
    //  ss->setRenderBinDetails( this->renderBin(), "RenderBin" );
    //}

    ss->setMode ( GL_BLEND,      osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE );
    ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );  
    ss->setAttribute ( mat.get(), osg::StateAttribute::ON );
    ss->setRenderBinDetails( this->renderBin(), "RenderBin" );

    //if( this->tableName() != "valley_water")
    {
      osg::ref_ptr< osg::PolygonOffset > offset ( new osg::PolygonOffset( -1.0f, -1.0f ) );
      ss->setMode ( GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
      ss->setAttribute( offset.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
    }

    //osg::ref_ptr< osg::Depth > depth ( new osg::Depth );
    //depth->setRange( 0.1, 1.0 );
    //ss->setAttribute( depth.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
  }

  return _group.get();
}

