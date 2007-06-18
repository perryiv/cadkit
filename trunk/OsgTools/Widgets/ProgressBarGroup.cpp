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
//
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarGroup::ProgressBarGroup() : 
_master ( new ProgressBar() ),
_slave ( new ProgressBar() ),
_numItems ( 1 ), _currentItem ( 0 ),
_currItemMin ( 0.0f ), _currItemMax ( 100.0f ), _currItemVal ( 0.0f ),
_border ( new osg::Node() ),
_pos( osg::Vec3f ( 0.0f, 0.0f, -3.1f ) ),
_ll ( osg::Vec2f( 0.0f, 0.0f ) ),
_borderZOffset ( -0.0003f ),
_isRelativeToAbsolute ( true )
{
  _group = new osg::Group();
  this->_init();
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
//
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ProgressBarGroup::getProgressBarGroup()
{
  return _group.get();

}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setRelativeToAbsolute( bool value )
{
  _isRelativeToAbsolute = value;
  this->_buildProgressBarGroup();
  
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setCurrentItemMin( double min )
{
  _currItemMin = min;
  _slave->setMin( min );
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setMessage ( const std::string& m )
{
  _slaveText = m;
  _slave->setMessage( m );
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setCurrentItemMax( double max )
{
  _currItemMax = max;
  _slave->setMax( max );
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::reset()
{
  _currItemVal = _currItemMin;
  _slave->reset();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setCurrentItemValue( double v )
{
  _currItemVal = v;
  _slave->setCurrent( v );
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setNumItems ( int n )
{
  _numItems = n;
  _master->setMax( float( n ) );
  this->_updateMasterMessage();
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setCurrentItem ( int i )
{
  _currentItem = i;
  _master->setCurrent( float ( i ) );
  this->_updateMasterMessage();
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::setPosition ( const osg::Vec3f & p )
{
  _pos = p;
  this->_buildProgressBarGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::updateProgressBarGroup()
{
  
}


////////////////////////////
// Protected Methods
////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::_init()
{
  float length, height;

  if( _master->getBorderLength() >= _slave->getBorderLength() )
    length = _master->getBorderLength();
  else
    length = _slave->getBorderLength();

  if( _master->getBorderHeight() >= _slave->getBorderHeight() )
    height = _master->getBorderHeight();
  else
    height = _slave->getBorderHeight();

  _padding = height * .15;

  _slave->setLowerLeft( osg::Vec2f ( _ll.x() + _padding, _ll.y() + _padding ) );
  _master->setLowerLeft( osg::Vec2f ( _ll.x() + _padding, _slave->getLowerLeft().y() + _slave ->getBorderHeight() + _padding * 2) );
  
  _length = length + _padding * 4;
  _height = ( height * 2 + _padding * 6 );
  _ll.x() -= _padding;
  _ll.y() -= _padding;

  _master->setMin ( 0.0f );
  _master->setMax ( float ( _numItems ) );
  _master->setCurrent ( float ( _currentItem ) );

  _slave->setMin ( _currItemMin );
  _slave->setMax ( _currItemMax );
  _slave->setCurrent ( _currItemVal );

  this->_updateMasterMessage();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::_updateMasterMessage()
{
  std::ostringstream text;
  text << "Loading item: " << _currentItem << " of " << _numItems;
  _masterText = text.str().c_str();
  _master->setMessage( _masterText );

}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
void ProgressBarGroup::_buildProgressBarGroup()
{
  osg::ref_ptr< osg::MatrixTransform > matrix ( new osg::MatrixTransform() );

  matrix->setMatrix( osg::Matrix::translate( _pos.x(), _pos.y(), _pos.z() ) );

  matrix->addChild( _master->getProgressBar() );
  matrix->addChild( _slave->getProgressBar() );
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
//
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
