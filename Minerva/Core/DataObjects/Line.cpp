
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
//  Class to represent line data.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/DataObjects/Line.h"
#include "Minerva/Core/DataObjects/UserData.h"
#include "Minerva/Core/Visitor.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/ILineData.h"
#include "Usul/Interfaces/IFitLineTerrain.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Font.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Depth"
#include "osg/Material"
#include "osg/Version"

using namespace Minerva::Core::DataObjects;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::Line() :
BaseClass(),
_width ( 1.0 ),
_node ( new osg::Group )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::~Line()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Line::accept ( Minerva::Core::Visitor& visitor )
{
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the width.
//
///////////////////////////////////////////////////////////////////////////////

float Line::width () const
{
  return _width;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the width.
//
///////////////////////////////////////////////////////////////////////////////

void Line::width ( float width )
{
  _width = width;
  this->dirty( true );
}


namespace Detail
{
  template < class Vertices >
  osg::Vec3Array* convert ( const Vertices& in, Usul::Interfaces::IUnknown *caller )
  {
    osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
    vertices->reserve ( in.size() );

    Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
    if ( planet.valid() )
    {
      for ( typename Vertices::const_iterator iter = in.begin(); iter != in.end(); ++iter )
      {
        Usul::Math::Vec3d point;
        planet->convertToPlanet ( *iter, point );
        vertices->push_back ( osg::Vec3 ( point[0], point[1], point[2] ) );
      }
    }

    return vertices.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Line::_preBuildScene ( Usul::Interfaces::IUnknown* caller )
{
  if ( this->dirty() )
  {
    // Remove the drawables we have.
    _node->removeChild( 0, _node->getNumChildren() );

    //_node->setUserData( new UserData( this ) );

    osg::ref_ptr < osg::StateSet > ss ( _node->getOrCreateStateSet() );
    ss->setRenderBinDetails( this->renderBin(), "RenderBin" );

    Usul::Interfaces::ILineData::QueryPtr line ( this->geometry() );

    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    _node->addChild( geode.get() );

    if( line.valid () )
    {
      typedef Usul::Interfaces::ILineData::Vertices Vertices;
      typedef Usul::Components::Manager PluginManager;

      // Get the line data.
      Vertices data ( line->lineData() );

      Usul::Interfaces::IFitLineTerrain::QueryPtr fit ( PluginManager::instance().getInterface ( Usul::Interfaces::IFitLineTerrain::IID ) );

      Vertices sampledPoints;
      if ( fit.valid() )
        fit->resample( data, sampledPoints, 5 );
      else
        sampledPoints = data;

      osg::ref_ptr< osg::Vec3Array > vertices ( Detail::convert ( sampledPoints, caller ) );

      // Create the geometry
      osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

      // Add the primitive set
      geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );

      // Set the vertices.
	    geometry->setVertexArray( vertices.get() );

      // Set the colors.
      osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );
      colors->push_back( this->color() );
      geometry->setColorArray( colors.get() );
      geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

      geode->addDrawable ( geometry.get() );

      // Turn off lighting.
      OsgTools::State::StateSet::setLighting  ( _node.get(), false );
      OsgTools::State::StateSet::setLineWidth ( _node.get(), _width );

      geometry->dirtyDisplayList();
    }

    // Do we have a label?
    if( this->showLabel() && !this->label().empty() )
      _node->addChild( this->_buildLabel() );

    this->dirty( false );
  }

  return _node.get();
}

