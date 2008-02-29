
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
#include "Minerva/Core/Visitor.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPolygonData.h"
#include "Usul/Interfaces/ITriangulate.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"

#include "osg/Material"
#include "osg/PolygonOffset"
#include "osg/Geode"
#include "osg/Geometry"

using namespace Minerva::Core::DataObjects;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Polygon::Polygon() :
BaseClass(),
_showBorder( false ),
_showInterior ( true )
{
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
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Polygon::accept ( Minerva::Core::Visitor& visitor )
{
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a mesh.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Polygon::_buildPolygons( Usul::Interfaces::IUnknown* caller )
{
  typedef Usul::Components::Manager      PluginManager;
  typedef Usul::Interfaces::ITriangulate ITriangulate;
  typedef Usul::Interfaces::IPolygonData IPolygonData;
  typedef IPolygonData::Vertices         Vertices;
  typedef IPolygonData::Boundaries       Boundaries;
  
  // Get needed interfaces.
  ITriangulate::QueryPtr triangulate ( PluginManager::instance().getInterface ( ITriangulate::IID ) );
  IPolygonData::QueryPtr polygon ( this->geometry() );
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );

  // Make sure we have them...
  if( polygon.valid() && triangulate.valid() && planet.valid() )
  {
    Vertices outerBoundary ( polygon->outerBoundary() );
    Boundaries innerBoundaries ( polygon->innerBoundaries() );
    
    Vertices out;
    triangulate->triangulate ( outerBoundary, out );
    
    // Vertices and normals.
    osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
    osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array );
    
    // Reserve enough rooms.
    vertices->reserve( out.size() );
    normals->reserve( out.size() );
    
    for ( Vertices::const_iterator iter = out.begin(); iter != out.end(); ++iter )
    {
      Vertices::value_type v0 ( *iter );
      Vertices::value_type v1 ( *( iter + 1 ) );
      Vertices::value_type v2 ( *( iter + 2 ) );
      
      Vertices::value_type p0, p1, p2;
      
      planet->convertToPlanet ( v0, p0 );
      planet->convertToPlanet ( v1, p1 );
      planet->convertToPlanet ( v2, p2 );
      
      vertices->push_back ( osg::Vec3 ( p0[0], p0[1], p0[2] ) );
      vertices->push_back ( osg::Vec3 ( p1[0], p1[1], p1[2] ) );
      vertices->push_back ( osg::Vec3 ( p2[0], p2[1], p2[2] ) );
      
      Vertices::value_type n0 ( p0 ); n0.normalize();
      Vertices::value_type n1 ( p1 ); n1.normalize();
      Vertices::value_type n2 ( p2 ); n2.normalize();
      
      normals->push_back ( osg::Vec3 ( n0[0], n0[1], n0[2] ) );
      normals->push_back ( osg::Vec3 ( n1[0], n1[1], n1[2] ) );
      normals->push_back ( osg::Vec3 ( n2[0], n2[1], n2[2] ) );
    }
    
    osg::ref_ptr < osg::Geometry > geom ( new osg::Geometry );
    
    geom->setVertexArray ( vertices.get() );
    geom->setNormalArray ( normals.get() );
    geom->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );

    geom->addPrimitiveSet ( new osg::DrawArrays ( GL_TRIANGLES, 0, vertices->size() ) );

    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    geode->addDrawable( geom.get() );
    
    osg::Vec4 color ( this->color() );
    osg::ref_ptr < osg::Material > mat ( new osg::Material );
    mat->setDiffuse ( osg::Material::FRONT_AND_BACK, color );

    osg::ref_ptr < osg::StateSet > ss ( geode->getOrCreateStateSet () );

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

    geode->setUserData ( new Minerva::Core::DataObjects::UserData ( this ) );
    return geode.release();
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Polygon::_preBuildScene ( Usul::Interfaces::IUnknown* caller )
{
  // Clear what we have by making a new one.
  osg::ref_ptr < osg::Group > group ( new osg::Group );

  if( this->showInterior() )
  {
    group->addChild( this->_buildPolygons( caller ) );
  }

  if( this->showBorder() )
  {
    group->addChild( BaseClass::_preBuildScene( caller ) );
  }

  this->dirty( false );
  return group.release();
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
