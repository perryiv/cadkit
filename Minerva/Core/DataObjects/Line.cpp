
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

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Font.h"

#include "osg/Geometry"
#include "osg/Depth"
#include "osg/Material"
#include "osg/Version"

#include "osgText/Text"

using namespace Minerva::Core::DataObjects;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::Line() :
BaseClass(),
_width ( 1.0 ),
_node ( new osg::Geode )
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
#if OSG_VERSION_MAJOR == 1 && OSG_VERSION_MINOR >= 1
    _node->removeDrawables( 0, _node->getNumDrawables() );
#else
    _node->removeDrawable( 0, _node->getNumDrawables() );
#endif
    _node->setUserData( new UserData( this ) );

    osg::ref_ptr < osg::StateSet > ss ( _node->getOrCreateStateSet() );
    ss->setRenderBinDetails( this->renderBin(), "RenderBin" );

    osg::ref_ptr < osg::Geometry > geometry ( this->geometry()->buildGeometry() );

    // Set the colors.
    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );
    colors->push_back( this->color() );
    geometry->setColorArray( colors.get() );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    _node->addDrawable ( geometry.get() );

    // Turn off lighting.
    OsgTools::State::StateSet::setLighting ( _node.get(), false );

    OsgTools::State::StateSet::setLineWidth ( _node.get(), _width );

    // Do we have a label?
    if( !this->label().empty() )
    {
      osg::ref_ptr < osgText::Text > text ( new osgText::Text );

      text->setFont( OsgTools::Font::defaultFont() );
      text->setColor( this->labelColor() );
      text->setPosition ( this->labelPosition() );
      text->setAutoRotateToScreen(true);
      text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
      text->setCharacterSize( this->labelSize() );

      text->setText ( this->label() );

      _node->addDrawable( text.get() );
    }

    geometry->dirtyDisplayList();

    this->dirty( false );
  }

  return _node.get();
}

