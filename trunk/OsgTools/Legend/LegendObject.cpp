
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Legend/LegendObject.h"

#include "osg/ref_ptr"
#include "osg/Group"
#include "osg/MatrixTransform"

using namespace OsgTools::Legend;


LegendObject::LegendObject() : BaseClass(),
_icon ( 0x0 ),
_text ( 0x0 ),
_width ( 0 ),
_height ( 0 )
{
}

LegendObject::~LegendObject()
{
}


void LegendObject::icon( Icon* icon )
{
  _icon = icon;
}

Icon* LegendObject::icon()
{
  return _icon.get();
}

const Icon* LegendObject::icon() const
{
  return _icon.get();
}

void LegendObject::text( Text* text )
{
  _text = text;
}

Text* LegendObject::text()
{
  return _text.get();
}

const Text* LegendObject::text() const
{
  return _text.get();
}

osg::Node* LegendObject::buildScene()
{
  osg::ref_ptr < osg::Group > group ( new osg::Group );

  unsigned int padding ( 5 );
  unsigned int iconWidth ( _width * 0.20 );

  if( _icon.valid() )
  {
    _icon->width( iconWidth );
    _icon->height( _height - ( padding * 2 ) );
    group->addChild( _icon->buildScene() );
  }

  if( _text.valid() )
  {
    osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
    osg::Matrix m ( osg::Matrix::translate ( iconWidth + padding, 0.0, 0.0 ) );
    mt->setMatrix( m );
    mt->addChild ( _text->buildScene() );
    group->addChild( mt.get() );
  }

  return group.release();
}

/// Set the size.
void LegendObject::size( unsigned int width, unsigned int height )
{
  _width = width;
  _height = height;
}
