
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Legend/Legend.h"
#include "OsgTools/Legend/Text.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include <algorithm>

using namespace OsgTools::Legend;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Legend::Legend() : 
  BaseClass(),
  _legendObjects(),
  _width ( 0 ),
  _height ( 0 ),
  _heightPerItem ( 0 ),
  _growDirection ( UP )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Legend::~Legend()
{
  _legendObjects.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::clear()
{
  _legendObjects.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a legend object.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::addRow( OsgTools::Legend::LegendObject *legendObject )
{
  _legendObjects.push_back( legendObject );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate to sort legend objects.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct SortLegend
  {
    SortLegend ( unsigned int column ) : _column ( column )
    {
    }
    
    template < class T >
    bool operator () ( const T& rhs, const T& lhs ) const
    {
      const Text* text0 ( dynamic_cast<const Text*> ( rhs->at( _column ) ) );
      const Text* text1 ( dynamic_cast<const Text*> ( lhs->at( _column ) ) );
      
      if ( 0x0 != text0 && 0x0 != text1 )
      {
        // Greater than because the first item is drawn on the bottom of the legend.
        // Sorting from Z to A will display A to Z on the legend.
        return text0->text() > text1->text();
      }
      
      return false;
    }
    
  private:
    unsigned int _column;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Legend::buildScene()
{
  //const SizeType width  ( this->size()[0] );
  //const SizeType height ( this->size()[1] );
  
  // Sort the legend by name.
  std::sort( _legendObjects.begin(), _legendObjects.end(), Detail::SortLegend ( 1 ) );

  // Create the root for the legend.
  osg::ref_ptr< osg::MatrixTransform > root ( new osg::MatrixTransform );
  root->setName( "Legend" );
  root->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
  root->setMatrix ( osg::Matrix::translate ( this->x(), this->y(), 0.0 ) );

  osg::ref_ptr < osg::StateSet > ss ( root->getOrCreateStateSet () );
  ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
  //ss->setMode ( GL_LIGHTING,  osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  OsgTools::State::StateSet::setTwoSidedLighting ( ss.get(), true );

  osg::ref_ptr< osg::Group > group ( new osg::Group );

  root->addChild ( group.get() );

  // Only build the legend if we have something...
  if( _legendObjects.size() > 0 )
  {
    // Calculate needed height values.
    unsigned int heightPerObject ( this->heightPerItem() );
    unsigned int height ( heightPerObject * _legendObjects.size() );

    // Change the height per object to fit in the size paramaters given.
    if( height > _height )
    {
      heightPerObject = this->_height / _legendObjects.size();
      height = _height;
    }

    // Build the background
    group->addChild ( this->_buildBackground( _width, height ) );

    const unsigned int padding ( 5 );

    for( LegendObjects::iterator iter = _legendObjects.begin(); iter != _legendObjects.end(); ++iter )
    {
      Item::RefPtr item ( *iter );
      
      if ( item.valid() )
      {
        // Transform for the row.
        osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform );
        
        // Width and height of the row.
        const unsigned int rowWidth ( _width - ( padding * 2 ) );
        const unsigned int rowHeight ( heightPerObject );
        
        const unsigned int num ( iter - _legendObjects.begin() );

        /// Y position (Needs to be signed).
        int yTranslate ( heightPerObject * num + padding );

        // Invert the y translation if we are growing down.
        if( this->growDirection() == DOWN )
          yTranslate *= -1;

        // Position the row.
        mt->setMatrix( osg::Matrix::translate ( padding, yTranslate, -1.0 ) );

        // Build the row.
        item->size ( rowWidth, rowHeight );
        mt->addChild( item->buildScene() );

        // Turn off depth testing.
        osg::ref_ptr < osg::StateSet > ss ( mt->getOrCreateStateSet() );
        ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

        // Add to the scene.
        group->addChild( mt.get() );
      }
    }
  }

  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size of the legend.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::maximiumSize( unsigned int width, unsigned int height )
{
  _width = width;
  _height = height;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the grow direction mode.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::growDirection( OsgTools::Legend::Legend::GrowDirectionMode mode )
{
  _growDirection = mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the grow direction mode.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Legend::Legend::GrowDirectionMode Legend::growDirection() const
{
  return _growDirection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the height per item.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::heightPerItem ( unsigned int height )
{
  _heightPerItem = height;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the height per item.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Legend::heightPerItem() const
{
  return _heightPerItem;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the height.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Legend::height() const
{
  unsigned int height ( _heightPerItem * _legendObjects.size() );

  return ( ( height > _height ) ? _height : height );
}
