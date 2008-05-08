
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
#include "Minerva/Interfaces/ILineData.h"
#include "Minerva/Interfaces/IFitLineTerrain.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Font.h"
#include "OsgTools/Utilities/TranslateGeometry.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Depth"
#include "osg/Material"
#include "osg/MatrixTransform"
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
  // Default render bin.
  this->renderBin ( 3 );
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

    _node->setUserData( new Minerva::Core::DataObjects::UserData( this ) );

    osg::ref_ptr < osg::StateSet > ss ( _node->getOrCreateStateSet() );

    // Query for needed interfaces.
    Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
    Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
    Minerva::Interfaces::ILineData::QueryPtr line ( this->geometry() );

    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    _node->addChild( geode.get() );

    if( line.valid () )
    {
      typedef Minerva::Interfaces::ILineData::Vertices Vertices;
      typedef Usul::Components::Manager PluginManager;

      // Get the line data.
      Vertices data ( line->lineData() );

      Minerva::Interfaces::IFitLineTerrain::QueryPtr fit ( PluginManager::instance().getInterface ( Minerva::Interfaces::IFitLineTerrain::IID ) );

      Vertices sampledPoints;
      if ( fit.valid() && DataObject::ABSOLUTE_MODE != this->altitudeMode() )
        fit->resample( data, sampledPoints, 5 );
      else
        sampledPoints = data;

      // Make the osg::Vec3Array.
      osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
      vertices->reserve ( sampledPoints.size() );
      
      if ( planet.valid() )
      {
        for ( Vertices::const_iterator iter = sampledPoints.begin(); iter != sampledPoints.end(); ++iter )
        {
          Vertices::value_type v ( *iter );
          
          // Get the height.
          v[2] = this->_elevation ( v, elevation.get() );
          
          Usul::Math::Vec3d point;
          planet->convertToPlanet ( v, point );
          vertices->push_back ( osg::Vec3 ( point[0], point[1], point[2] ) );
        }
      }

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

      // Get the state set.
      osg::ref_ptr<osg::StateSet> ss ( geometry->getOrCreateStateSet() );

      // Turn off lighting.
      OsgTools::State::StateSet::setLighting  ( ss.get(), false );
      OsgTools::State::StateSet::setLineWidth ( ss.get(), _width );

      // Turn on blending if we are semi-transparent.
      if ( this->color().w() < 1.0f )
      {
        ss->setMode ( GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
      }

      // Set the render bin.
      ss->setRenderBinDetails( this->renderBin(), "RenderBin" );

      geometry->dirtyDisplayList();

      // Set depth parameters.
      osg::ref_ptr<osg::Depth> depth ( new osg::Depth ( osg::Depth::LEQUAL, 0.0, 1.0, false ) );
      ss->setAttributeAndModes ( depth.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
    }

    // Do we have a label?
    if( this->showLabel() && !this->label().empty() )
    {
      osg::Vec3 position ( this->labelPosition() );
      Usul::Math::Vec3d p ( position[0], position[1], position[2] );
      
      if ( planet.valid() )
        planet->convertToPlanet ( Usul::Math::Vec3d ( p ), p );
      
      _node->addChild( this->_buildLabel( osg::Vec3 ( p[0], p[1], p[2] )  ) );
    }

    this->dirty( false );
  }

  osg::Vec3 offset ( _node->getBound().center() );
  osg::ref_ptr<OsgTools::Utilities::TranslateGeometry> tg ( new OsgTools::Utilities::TranslateGeometry ( offset ) );
  _node->accept ( *tg );

  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( offset ) );
  mt->addChild ( _node.get() );

  return mt.release();
}
