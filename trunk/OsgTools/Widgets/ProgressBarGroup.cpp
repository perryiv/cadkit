///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "ProgressBarGroup.h"

#include "osgDB/ReadFile"
#include "osg/MatrixTransform"
#include "osg/Math"
#include "osg/Texture2D"
#include "osg/Image"
#include "osg/Geometry"
#include "osg/Geode"
#include "osgText/Font"
#include "osgText/Text"

#include <iostream>
#include <sstream>

using namespace std;
using namespace OsgTools::Widgets;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarGroup::ProgressBarGroup() : 
_master ( new ProgressBar() ),
_slave ( new ProgressBar() ),
//_numItems ( 1 ), _currentItem ( 0 ),
//_currItemMin ( 0.0f ), _currItemMax ( 100.0f ), _currItemVal ( 0.0f ),
_border ( new osg::Node() ),
_pos( osg::Vec3f ( 0.0f, 0.0f, -3.1f ) ),
_ll ( osg::Vec2f( 0.0f, 0.0f ) ),
_padding ( 0.01 ),
_height ( 0.01 ),
_length ( 0.01 ),
_numBars ( 0 ),
_borderZOffset ( -0.0003f ),
_isRelativeToAbsolute ( true )
{
  _group = new osg::Group();
  //this->_init();
  this->_buildProgressBarGroup();
}

ProgressBarGroup::~ProgressBarGroup()
{
	
}

////////////////////////////
// Public Methods
////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get the "progress bar group" group
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ProgressBarGroup::getProgressBarGroup()
{
  return _group.get();

}

///////////////////////////////////////////////////////////////////////////////
//
//  Set RelativeToAbsolute flag for rendering
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setRelativeToAbsolute( bool value )
{
  _isRelativeToAbsolute = value;
  this->_buildProgressBarGroup();
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the minimum item on the bar at index i
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setItemMin( int i, double min )
{
  if( _numBars >= i )
    _pbarVector.at ( i )->setMin ( min );

  _slave->setMin( min );
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the progress bar message on the bar at index i
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setMessage ( int i, const std::string& m )
{
  if( _numBars >= i )
    _pbarVector.at ( i )->setMessage ( m );

  _slave->setMessage( m );
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max value on the bar at index i
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setItemMax( int i, double max )
{
  if( _numBars >= i )
    _pbarVector.at ( i )->setMax ( max );

  _slave->setMax( max );
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the bar at index i parameters to their starting values
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::resetBar( int i )
{
  if( _numBars >= i )
    _pbarVector.at ( i )->reset();

  _slave->reset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current progression value on the bar at index i
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setItemValue( int i, double v )
{
  if( _numBars >= i )
    _pbarVector.at ( i )->setCurrent ( v );

  _slave->setCurrent( v );
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of item on the master bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setNumItems ( int n )
{
  _master->setMax( float( n ) );
  _numBars = n;
  this->_updateMasterMessage();
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current item on the master bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setCurrentItem ( int i )
{
  _master->setCurrent( float ( i ) );
  this->_updateMasterMessage();
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the position on the screen for the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setPosition ( const osg::Vec3f & p )
{
  _pos = p;
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the amount of padding between bar objects
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setPadding ( float p )
{
  _padding = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::updateProgressBarGroup()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a progress bar object to the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::add ( ProgressBar* pbar )
{
  if( _numBars == 0 )
    pbar->setLowerLeft( osg::Vec2f ( _ll.x() + _padding, _ll.y() + _height ) );
  else
    pbar->setLowerLeft( osg::Vec2f ( _ll.x() + _padding,
                                     _pbarVector.at( _numBars - 1 )->getLowerLeft().y() + 
                                     _pbarVector.at( _numBars - 1 )->getHeight() + 
                                     _padding ) );
  ++_numBars;
  if(_length < pbar->getLength() + _padding )
    _length = pbar->getLength() + _padding * 2;

  _height +=  pbar->getHeight() + _padding;

  _pbarVector.push_back ( pbar );

  this->_buildProgressBarGroup();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Create and add a progress bar object to the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::add ( const std::string& m, double min, double max )
{
  ProgressBar::RefPtr pbar = new ProgressBar( );
  pbar->setMessage ( m );
  pbar->setMin ( min );
  pbar->setMax ( max );
  pbar->setCurrent ( min );
  if( _numBars == 0 )
    pbar->setLowerLeft( osg::Vec2f ( _ll.x() + _padding, _ll.y() + _height ) );
  else
    pbar->setLowerLeft( osg::Vec2f ( _ll.x() + _padding,
                                     _pbarVector.at( _numBars - 1 )->getLowerLeft().y() + 
                                     _pbarVector.at( _numBars - 1 )->getHeight() + 
                                     _padding ) );
  ++_numBars;
  if(_length < pbar->getLength() + _padding )
    _length = pbar->getLength() + _padding * 2;

  _height +=  pbar->getHeight() + _padding;

  
  
  _pbarVector.push_back ( pbar );
  
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  TODO
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::remove ( int pos )
{
  //TODO
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the location of the progress bar on the screen.  Designed to work with
//  relative to absolute flag set.  Use setPosition when relative to absolute
//  is false.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setLocation ( unsigned int loc )
{
  switch( loc )
  {
    case LOWER_LEFT : 
      this->setPosition ( osg::Vec3f ( -0.95 , -.75, -3.0f ) );
      break;
    case LOWER_RIGHT : 
      this->setPosition ( osg::Vec3f ( 0.50 * .7, -.75, -3.0f ) );
      break;
    case UPPER_LEFT : 
      this->setPosition ( osg::Vec3f ( -0.95 * .7, .4, -3.0f ) );
      break;
    case UPPER_RIGHT : 
      this->setPosition ( osg::Vec3f ( 0.50  * .7, .4, -3.0f ) ); 
      break;
    case CENTER : 
      this->setPosition ( osg::Vec3f ( -.25f, -.4, -3.0f ) );
      break;
    default:
      break;
  }

}


////////////////////////////
// Protected Methods
////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the master and slave bars to starting values.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::_init()
{
 /* float length, height;

  if( _master->getBorderLength() >= _slave->getBorderLength() )
    length = _master->getBorderLength();
  else
    length = _slave->getBorderLength();

  if( _master->getBorderHeight() >= _slave->getBorderHeight() )
    height = _master->getBorderHeight();
  else
    height = _slave->getBorderHeight();

  _padding = 0.05f;

  _slave->setLowerLeft( osg::Vec2f ( _ll.x() + _padding, _ll.y() + _padding ) );
  _master->setLowerLeft( osg::Vec2f ( _ll.x() + _padding, _slave->getLowerLeft().y() + _slave ->getBorderHeight() + _padding * 2) );
  
  _length = length + _padding * 4;
  _height = ( height * 2 + _padding * 6 );
  _ll.x() -= _padding;
  _ll.y() -= _padding;

  _master->setMin ( 0.0f );
  _master->setMax ( 1.0f );
  _master->setCurrent ( 0.0f );

  _slave->setMin ( 0.0f );
  _slave->setMax ( 100.0f );
  _slave->setCurrent ( 0.0f );

  this->_updateMasterMessage();*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar message on the master bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::_updateMasterMessage()
{
  std::ostringstream text;
  text << "Loading item: " << _master->getCurrent() << " of " << _master->getMax();
  _master->setMessage( text.str().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the "Progress Bar Group" group.  Apply matrix transforms
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::_buildProgressBarGroup()
{
  osg::ref_ptr< osg::MatrixTransform > matrix ( new osg::MatrixTransform() );

  matrix->setMatrix( osg::Matrix::translate( _pos.x(), _pos.y(), _pos.z() ) );

  
  for( int x = 0; x < _numBars ; ++x  )
  {
    matrix->addChild ( _pbarVector.at ( x )->getProgressBar() );
  }
    
  //matrix->addChild( _master->getProgressBar() );
  //matrix->addChild( _slave->getProgressBar() );
  matrix->addChild( this->_buildBar( 999,
                                     "icons/borderGroup.tga",
                                     osg::Vec2f ( _ll.x(), _ll.y() + _height ),
                                     osg::Vec2f ( _ll.x() + _length, _ll.y() ),
                                     _borderZOffset ) );
  if( _isRelativeToAbsolute )
    matrix->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );

  _group->removeChildren ( 0, _group->getNumChildren() ); 

  _group->addChild( matrix.release() );
  
}
///////////////////////////////////////////////////////////////////////////////
//
//  Build a progress bar object.  Used to build the progress bar group
//  background.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ProgressBarGroup::_buildBar( int render_level , std::string tex, const osg::Vec2f& ul, const osg::Vec2f& lr, float depth  )
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode() );

  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry() );

  osg::ref_ptr< osg::StateSet > stateset ( geometry->getOrCreateStateSet() );
	osg::ref_ptr< osg::Image > image = osgDB::readImageFile( tex );

	if ( image.get() )
  {
	  osg::ref_ptr< osg::Texture2D > texture ( new osg::Texture2D() ); 
	  texture->setImage ( image.get() );
	  stateset->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
	  stateset->setMode ( GL_REPLACE, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
	  stateset->setMode ( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );    
    stateset->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
    stateset->setRenderBinDetails ( render_level, "RenderBin" );
  }

  osg::ref_ptr< osg::Vec4Array > white ( new osg::Vec4Array() );  
  white->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

  osg::ref_ptr< osg::Vec3Array > normal ( new osg::Vec3Array() );
  normal->push_back ( osg::Vec3 ( 0.0f, 0.0f, 1.0f ) );

  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array() );
  vertices->reserve( 4 );
  vertices->push_back ( osg::Vec3f ( ul.x(), lr.y(), depth ) );
  vertices->push_back ( osg::Vec3f ( lr.x(), lr.y(), depth ) );
  vertices->push_back ( osg::Vec3f ( lr.x(), ul.y(), depth ) );
  vertices->push_back ( osg::Vec3f ( ul.x(), ul.y(), depth ) );

  osg::Vec2 texCoords[] =
  {
    osg::Vec2(0.0f, 0.0f),
    osg::Vec2(1.0f, 0.0f),
    osg::Vec2(1.0f, 1.0f),
    osg::Vec2(0.0f, 1.0f),            
  };

  osg::ref_ptr< osg::Vec2Array > uvcoords ( new osg::Vec2Array( 4, texCoords ) );

  geometry->setTexCoordArray( 0 , uvcoords.get() );

  geometry->setVertexArray( vertices.get() );

  geometry->setColorArray( white.get() );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
        
  geometry->setNormalArray( normal.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

  geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );

  geode->addDrawable( geometry.get() );

  return geode.release();
}
