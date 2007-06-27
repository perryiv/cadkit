
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

#include "Usul/Interfaces/ILineData.h"

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

osg::Node* Line::buildScene()
{
  if ( this->dirty() )
  {
    // Remove the drawables we have.
    _node->removeChild( 0, _node->getNumChildren() );

    _node->setUserData( new UserData( this ) );

    osg::ref_ptr < osg::StateSet > ss ( _node->getOrCreateStateSet() );
    ss->setRenderBinDetails( this->renderBin(), "RenderBin" );

    Usul::Interfaces::ILineData::QueryPtr line ( this->geometry() );

    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    _node->addChild( geode.get() );

    if( line.valid () )
    {
      osg::ref_ptr < osg::Geometry > geometry ( line->buildLineData() );

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

