
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/Legend.h"
#include "OsgTools/Widgets/Text.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include <algorithm>

using namespace OsgTools::Widgets;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Legend::Legend() : 
  BaseClass(),
  _legendObjects(),
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

void Legend::addRow( OsgTools::Widgets::LegendObject *legendObject )
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
  const SizeType maxWidth  ( this->maximiumSize()[0] );
  const SizeType maxHeight ( this->maximiumSize()[1] );
  
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
    // Padding around legend.
    const unsigned int padding ( 5 );
    
    // Constant row width.
    const unsigned int rowWidth ( maxWidth - ( padding * 2 ) );
    
    // Calculate needed height values.
    SizeType totalHeight ( 0 );
    std::vector<SizeType> heights;
    heights.reserve ( _legendObjects.size() );
    
    for ( LegendObjects::iterator iter = _legendObjects.begin(); iter != _legendObjects.end(); ++iter )
    {
      Item::RefPtr item ( *iter );
      
      if ( item.valid() )
      {
        item->size ( rowWidth, static_cast<double> ( maxHeight ) / _legendObjects.size() );
        Size s ( item->estimateSizeForWidth( rowWidth ) );
        heights.push_back( s[1] );
        totalHeight += s[1];
      }
      else
        heights.push_back( 0 );
    }
    
    unsigned int height ( totalHeight );

    // Change the height per object to fit in the size paramaters given.
    if( totalHeight > maxHeight )
    {
      const double tooBig ( static_cast<double> ( height ) / maxHeight );
      for ( unsigned int i = 0; i < heights.size(); ++i )
      {
        heights[i] /= tooBig;
      }
      
      height = maxHeight;
    }

    // Build the background
    group->addChild ( this->_buildBackground( maxWidth, height ) );

    // The current height.
    unsigned int currentHeight ( padding );
    
    for( LegendObjects::iterator iter = _legendObjects.begin(); iter != _legendObjects.end(); ++iter )
    {
      Item::RefPtr item ( *iter );
      
      if ( item.valid() )
      {
        // Transform for the row.
        osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform );
        
        // Height of the row.
        const unsigned int num ( std::distance ( _legendObjects.begin(), iter ) );
        const unsigned int rowHeight ( heights.at ( num ) );
        
        /// Y position (Needs to be signed).
        int yTranslate ( currentHeight );

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
        
        // Increment the current height.
        currentHeight += rowHeight;
      }
    }
  }

  return root.release();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the grow direction mode.
//
///////////////////////////////////////////////////////////////////////////////

void Legend::growDirection( OsgTools::Widgets::Legend::GrowDirectionMode mode )
{
  _growDirection = mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the grow direction mode.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Widgets::Legend::GrowDirectionMode Legend::growDirection() const
{
  return _growDirection;
}
