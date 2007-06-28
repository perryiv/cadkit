
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "ProgressBarGroup.h"

#include "Usul/CommandLine/Arguments.h"

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
#include <algorithm>

using namespace OsgTools::Widgets;

///////////////////////////////////////////////////////////////////////////////
//
//  Update Callback for the progress bar group to remove finished progress
//  bars.
//
///////////////////////////////////////////////////////////////////////////////

class UpdateProgressBarGroupCallback : public osg::NodeCallback
{
  public:
  typedef osg::Drawable::UpdateCallback   BaseClass;

  UpdateProgressBarGroupCallback::UpdateProgressBarGroupCallback ( OsgTools::Widgets::ProgressBarGroup * pbarGroup ) : 
  _pbarGroup ( pbarGroup )
  {
  }
  
  virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
  {
    if( 0x0 != _pbarGroup )
    {
      //int num = _pbarGroup->getNumItems();
      for( int i = 0; i < _pbarGroup->getNumItems(); ++i)
      {
        if( _pbarGroup->isItemFinished ( i ) && !_pbarGroup ->isAnimating( i ) && !_pbarGroup->isVisible( i ) )
        {
          _pbarGroup->remove ( i );
        }
      }
    }
    traverse(node,nv);
  }

protected:
  virtual UpdateProgressBarGroupCallback::~UpdateProgressBarGroupCallback()
  {
    _pbarGroup = 0x0;
  }
private:
 
  OsgTools::Widgets::ProgressBarGroup * _pbarGroup;
};



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarGroup::ProgressBarGroup() : 
_border ( new osg::Node() ),
_pos( osg::Vec3f ( -.25f, -.75, -3.0f ) ),
_ll ( osg::Vec2f( 0.0f, 0.0f ) ),
_padding ( 0.0 ),
_height ( 0.01 ),
_length ( 0.01 ),
_numBars ( 0 ),
_borderZOffset ( -0.0003f ),
_isRelativeToAbsolute ( true ),
_groupLH ( osg::Vec2f ( 1.0f, 0.03f ) )
{
  _group = new osg::Group();
  _group->setUpdateCallback( new UpdateProgressBarGroupCallback ( this ) ); 
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarGroup::~ProgressBarGroup()
{
  std::for_each ( _pbarVector.begin(), _pbarVector.end(), std::mem_fun ( &ProgressBar::clear ) );
}


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
//  Show the progress bar at index i
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::showProgressBar ( unsigned int index )
{
  if ( _numBars >= index  && _numBars > 0 )
    _pbarVector.at(index)->showProgressBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the progress bar at index i
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::hideProgressBar ( unsigned int index )
{
  if ( _numBars >= index  && _numBars > 0 )
    _pbarVector.at(index)->hideProgressBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the minimum item on the bar at index i
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setItemMin( unsigned int i, double min )
{
  if ( _numBars >= i  && _numBars > 0 )
    _pbarVector.at ( i )->setMin ( min );

  //this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the progress bar message on the bar at index i
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setMessage ( unsigned int i, const std::string& m )
{
  if ( _numBars >= i  && _numBars > 0 )
    _pbarVector.at ( i )->setMessage ( m );

  //this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max value on the bar at index i
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setItemMax( unsigned int i, double max )
{
  if ( _numBars >= i && _numBars > 0 )
    _pbarVector.at ( i )->setMax ( max );

  //this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the bar at index i parameters to their starting values
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::resetBar( unsigned int i )
{
  if ( _numBars >= i  && _numBars > 0 )
    _pbarVector.at ( i )->reset();

  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the global length for all bars in the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setGroupLength ( float l)
{
  if( l != _groupLH.x() )
  {
    _groupLH.x() = l;
    for( unsigned int i = 0; i < _numBars; ++i)
    {
      _pbarVector.at ( i )->setBarLength ( _groupLH.x() );
    }
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the global height for all bars in the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setGroupHeight ( float h )
{
  if( h != _groupLH.y() )
  {
    _groupLH.y() = h;
    for( unsigned int i = 0; i < _numBars; ++i)
    {
      _pbarVector.at ( i )->setBarHeight ( _groupLH.y() );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the global height and length for all bars in the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setGroupLengthAndHeight ( const osg::Vec2f& lh )
{
  if( lh.y() != _groupLH.y() || lh.x() != _groupLH.x() )
  {
    _groupLH.y() = lh.y();
    _groupLH.x() = lh.x();
    for( unsigned int i = 0; i < _numBars; ++i)
    {
      _pbarVector.at ( i )->setBarLengthAndHeight ( _groupLH );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current progression value on the bar at index i
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setItemValue( unsigned int i, double v )
{
  if ( _numBars >= i  && _numBars > 0 )
    _pbarVector.at ( i )->setCurrent ( v );

  //this->_buildProgressBarGroup();
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
//  Add a progress bar object to the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::add ( ProgressBar* pbar )
{
  pbar->setBarLengthAndHeight ( _groupLH );
  this->_addProgressBar ( pbar );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create and add a progress bar object to the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::add ( const std::string& m, double min, double max )
{
  ProgressBar::RefPtr pbar ( new ProgressBar() );
  pbar->setBarLengthAndHeight ( _groupLH );
  pbar->setMessage ( m );
  pbar->setMin ( min );
  pbar->setMax ( max );
  pbar->setCurrent ( min );

  this->_addProgressBar ( pbar.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create and add a progress bar object to the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ProgressBarGroup::append (  )
{
  ProgressBar::RefPtr pbar = new ProgressBar( );

  pbar->setBarLengthAndHeight ( _groupLH );
  this->_addProgressBar ( pbar.get() );

  return pbar->queryInterface ( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update (rebuild) the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::update()
{
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create and add a progress bar object to the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::_addProgressBar ( ProgressBar * pbar )
{
  if( _numBars == 0 )
    pbar->setLowerLeft ( osg::Vec2f ( _ll.x() + _padding, _ll.y() + _padding ) );
  else
    pbar->setLowerLeft ( osg::Vec2f ( _ll.x() + _padding,
                                     _pbarVector.at( _numBars - 1 )->getLowerLeft().y() + 
                                     _pbarVector.at( _numBars - 1 )->getHeight() + 
                                     _padding ) );
  ++_numBars;
  if (_length < pbar->getLength() + _padding )
    _length = pbar->getLength() + _padding * 2;

  if(_height == 0 )
    _height +=  pbar->getHeight() + _padding * 2;
  else
    _height +=  pbar->getHeight() + _padding;

  
  
  _pbarVector.push_back ( pbar );
  
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  TODO: Removes progress bar object at position pos
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::remove ( unsigned int pos )
{
  ProgressBar::RefPtr bar ( _pbarVector.at ( pos ) );
  if ( _numBars > 0 )
  {
    if ( _numBars == 1 )
    {
      _height = 0.00;
      _length = 0.00;
    }
    else
    {
      for ( unsigned int x = _numBars - 1; x > pos; --x)
      {
        _pbarVector.at( x )->setLowerLeft ( osg::Vec2f ( _ll.x() + _padding,
                                       _pbarVector.at( x - 1 )->getLowerLeft().y() ) );
      }
      _height -= bar->getHeight()  + _padding;
      
    }
    
    bar->clear();
    _pbarVector.erase( _pbarVector.begin() + pos );
    if( _numBars > 0 )
      _numBars --;
  }
  
  this->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  TODO: Removes progress bar object at position pos
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::clear()
{
  _group->removeChild ( 0 , _group->getNumChildren() );
  _group = 0x0;
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
    case BOTTOM :
      this->setPosition ( osg::Vec3f ( -.25f, -.75, -3.0f ) );
      break;
    default:
      break;
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the "Progress Bar Group" group.  Apply matrix transforms
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::_buildProgressBarGroup()
{
  osg::ref_ptr< osg::MatrixTransform > matrix ( new osg::MatrixTransform() );

  matrix->setMatrix ( osg::Matrix::translate( _pos.x(), _pos.y(), _pos.z() ) );

  
  for( unsigned int x = 0; x < _numBars ; ++x  )
  {
    matrix->addChild ( _pbarVector.at ( x )->getProgressBar() );
  }

  /*matrix->addChild ( this->_buildBar( 999,
                                     "/icons/borderGroup.tga",
                                     osg::Vec2f ( _ll.x(), _ll.y() + _height ),
                                     osg::Vec2f ( _ll.x() + _length, _ll.y() ),
                                     _borderZOffset ) );*/
 

  if ( _isRelativeToAbsolute )
    matrix->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );

  _group->removeChildren ( 0, _group->getNumChildren() ); 

  
  _group->addChild ( matrix.release() );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a progress bar object.  Used to build the progress bar group
//  background.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ProgressBarGroup::_buildBar( unsigned int render_level , std::string tex, const osg::Vec2f& ul, const osg::Vec2f& lr, float depth  )
{
  std::string directory ( Usul::CommandLine::Arguments::instance().directory() + tex );

  osg::ref_ptr< osg::Geode > geode ( new osg::Geode() );
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry() );
  osg::ref_ptr< osg::StateSet > stateset ( geometry->getOrCreateStateSet() );
  osg::ref_ptr< osg::Image > image = osgDB::readImageFile ( directory );

	if ( image.get() )
  {
	  osg::ref_ptr< osg::Texture2D > texture ( new osg::Texture2D() ); 
	  texture->setImage ( image.get() );
	  stateset->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }

  osg::ref_ptr< osg::Vec4Array > white ( new osg::Vec4Array() );  
  white->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

  osg::ref_ptr< osg::Vec3Array > normal ( new osg::Vec3Array() );
  normal->push_back ( osg::Vec3 ( 0.0f, 0.0f, 1.0f ) );

  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array() );
  vertices->reserve ( 4 );
  vertices->push_back ( osg::Vec3f ( ul.x(), lr.y(), depth ) );
  vertices->push_back ( osg::Vec3f ( lr.x(), lr.y(), depth ) );
  vertices->push_back ( osg::Vec3f ( lr.x(), ul.y(), depth ) );
  vertices->push_back ( osg::Vec3f ( ul.x(), ul.y(), depth ) );

  osg::ref_ptr< osg::Vec2Array > uvcoords ( new osg::Vec2Array( 4 ) );
  uvcoords->at ( 0 ) = osg::Vec2( 0.0f, 0.0f );
  uvcoords->at ( 1 ) = osg::Vec2( 1.0f, 0.0f );
  uvcoords->at ( 2 ) = osg::Vec2( 1.0f, 1.0f );
  uvcoords->at ( 3 ) = osg::Vec2( 0.0f, 1.0f );

  geometry->setTexCoordArray ( 0 , uvcoords.get() );

  geometry->setVertexArray ( vertices.get() );

  geometry->setColorArray ( white.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );
        
  geometry->setNormalArray ( normal.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );

  geometry->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );

  geode->addDrawable ( geometry.get() );

  return geode.release();
}
