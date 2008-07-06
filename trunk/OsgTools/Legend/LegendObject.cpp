
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


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LegendObject::LegendObject() : BaseClass(),
  _texts(),
  _percentages()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LegendObject::~LegendObject()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text at given index.
//
///////////////////////////////////////////////////////////////////////////////

Item* LegendObject::operator[] ( unsigned int i )
{
  if( i < _texts.size() )
    return _texts[i];

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text at given index.
//
///////////////////////////////////////////////////////////////////////////////

Item* LegendObject::at ( unsigned int i )
{
  if( i < _texts.size() )
    return _texts[i];

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text at given index.
//
///////////////////////////////////////////////////////////////////////////////

const Item* LegendObject::at ( unsigned int i ) const
{
  if( i < _texts.size() )
    return _texts[i];

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a text object.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int LegendObject::addItem ( Item* text )
{
  _texts.push_back( text );
  _percentages.push_back( 0.0 );

  /// Return the index the text was placed at.
  return _texts.size() - 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of columns.
//
///////////////////////////////////////////////////////////////////////////////

void LegendObject::columns( unsigned int value )
{
  _texts.resize( value );

  _percentages.resize( value );
  for( unsigned int i = 0; i < value; ++i )
    _percentages.at( i ) = static_cast < float > ( value ) / 100.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of rows.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int LegendObject::columns() const
{
  return _texts.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* LegendObject::buildScene()
{
  const SizeType width  ( this->size()[0] );
  const SizeType height ( this->size()[1] );
  
  osg::ref_ptr < osg::Group > group ( new osg::Group );

  const unsigned int padding ( 5 );
  
  // Maximum width and height for the row.
  const unsigned int maxWidth  ( static_cast < unsigned int > ( width ) );
  const unsigned int maxHeight ( height - ( padding * 2 ) );
  
  // Current x position of item.
  unsigned int currentPosition ( padding );
  
  // Loop over each item.
  for( unsigned int i = 0; i < _texts.size(); ++i )
  {
    const unsigned int columnWidth ( static_cast < unsigned int > ( maxWidth * _percentages.at( i ) ) );
    Item::RefPtr text ( _texts.at ( i ) );

    osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
    osg::Matrix m ( osg::Matrix::translate ( currentPosition, 0.0, 0.0 ) );
    mt->setMatrix( m );
    text->size ( columnWidth, maxHeight );
    mt->addChild ( text->buildScene() );
    group->addChild( mt.get() );

    currentPosition += ( columnWidth + padding );
  }

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get reference to percentage.
//
///////////////////////////////////////////////////////////////////////////////

float& LegendObject::percentage( unsigned int i )
{
  return _percentages.at( i );
}
