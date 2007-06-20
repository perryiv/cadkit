///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/ProgressBar.h"

#include "osgDB/ReadFile"
#include "osg/MatrixTransform"
#include "osg/Math"
#include "osg/Texture2D"
#include "osg/Image"
#include "osg/Geometry"
#include "osg/Geode"
#include "osgText/Font"
#include "osgText/Text"

//#include <iostream>
using namespace OsgTools::Widgets;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ProgressBar, ProgressBar::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
///////////////////////////////////////////////////////////////////////////////

ProgressBar::ProgressBar() :
BaseClass(),
_min ( 0.0 ),
_max ( 100.0 ),
_current ( 0.0 ),
_border ( new osg::Node() ),
_progressBar ( new osg::Node() ),
_backgroundBar ( new osg::Node() ),
_barText ( "" ),
_barSize ( 0.0f ),
_backSize ( 0.5f ),
_barLength ( 0.5f ),
_barHeight ( 0.05f ),
_borderLength ( 0.7f ),
_borderHeight ( 0.2f ),
_borderPadding ( 0.1f ),
_borderZOffset ( -0.0002f ),
_barBorderThickness ( 0.005f ),
_barBorderZOffset ( -0.0001f ),
_textZOffset ( 0.0001f ),
_isRelativeToAbsolute ( false ),
_isFinished ( false ),
_ll ( osg::Vec2f ( 0.0f, 0.0f ) )
{
  _borderPadding = ( _barHeight + _barBorderThickness ) * ( _barLength + _barBorderThickness) * .5;
  _borderHeight = _barHeight * 2.0f;
  _borderLength = _barLength * 1.15f;
  _pbarGroup = new osg::Group() ;
	this->_buildProgressBarObject();
  this->updateProgressBar();
}

ProgressBar::~ProgressBar()
{
	
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ProgressBar::getProgressBar()
{
  return _pbarGroup.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset progress bar values to the starting parameters
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::reset()
{
  _isFinished = false;
  _current = _min;
  this->_buildProgressBarObject();
  this->updateProgressBar();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Query for given interface id.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ProgressBar::queryInterface ( unsigned long iid )
{
	switch ( iid )
	{
	case Usul::Interfaces::IUnknown::IID:
	case Usul::Interfaces::IStatusBar::IID:
		return static_cast < Usul::Interfaces::IStatusBar * > ( this );
	case Usul::Interfaces::IProgressBar::IID:
		return static_cast < Usul::Interfaces::IProgressBar * > ( this );
	default:
		return 0x0;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::showProgressBar()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::hideProgressBar()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the total of progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setTotalProgressBar ( unsigned int value )
{
	this->setMax ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::updateProgressBar ( unsigned int value )
{
	this->setCurrent ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setStatusBarText ( const std::string &text, bool force )
{
	this->setMessage ( text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rebuild the progress bar objects after changes have been made.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::updateProgressBar()
{
  this->_buildProgressBarObject();
  this->_buildProgressBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the minimum value
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setMin( double min )
{
  _min = min;
  this->updateProgressBar();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum value
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setMax( double max )
{
  _max = max;
  this->updateProgressBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the hight of the progress bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setBarHeight ( float h )
{
  _barHeight = h;
  this->updateProgressBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the progress bar message
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setMessage ( const std::string& s )
{
  _barText = s;
  this->updateProgressBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lower left corner of the progress bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setLowerLeft( const osg::Vec2f & ll )
{
  _ll.x() = ll.x();
  _ll.y() = ll.y();
  this->updateProgressBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set RelativeToAbsolute flag for rendering
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setRelativeToAbsolute ( bool value )
{
  _isRelativeToAbsolute = value;
  this->updateProgressBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the length of the bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setBarLength ( float l )
{
  _barLength = l;
  //this->_buildProgressBarObject();
  this->updateProgressBar();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the current value
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setCurrent( double c )
{
  _current = c;
  
  if( _current >= _max )
  {
    _current = _max;
    _isFinished = true;
  }
  if( _current < _min )
    _current = _min;

  _barSize = _barLength * ( (_current + _min ) / _max );

  //this->_buildProgressBarObject();

  this->updateProgressBar();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Draw a text string <s> at position <p>
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode* ProgressBar::_drawTextAtPosition( const osg::Vec3f & p, const std::string & s )
{ 
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode() );
  /////////////////////////////////////////////////////////////
  osg::ref_ptr< osgText::Font > font = osgText::readFontFile("fonts/arial.ttf");
  osgText::Text* text = new osgText::Text;
  text->setFont( font.get() );
  
  text->setColor( osg::Vec4f (1.0f, 1.0f, 1.0f, 1.0f ) );
  text->setCharacterSize( 0.025f );
  text->setPosition( p );
  text->setLayout(osgText::Text::LEFT_TO_RIGHT);
  text->setText( s );
  geode->addDrawable(text);
  /////////////////////////////////////////////////////////////

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the percentage complete
//
///////////////////////////////////////////////////////////////////////////////
std::string ProgressBar::_getPercentComplete()
{
  char s[10];
  //std::string s;
//  std::ostringstream f;
 
  double percentage =  (_current + _min ) / _max;
  //f << "0.1f";
  percentage *= 100;
  sprintf( s , "%0.1lf%c" , percentage, '%' );
  //_barSize = _barLength * ( (_current + _min ) / _max );

  return s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the bar objects
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::_buildProgressBarObject()
{
  //build bar objects
  osg::ref_ptr< osg::Group > group ( new osg::Group() );

  this->_progressBar =  ( this->_buildBar( 1003,
                                           "icons/bar.tga",
                                           osg::Vec2f ( _ll.x(), _ll.y() + _barHeight ),
                                           osg::Vec2f(_ll.x() + _barSize , _ll.y()),
                                           0.0f ) );
  this->_backgroundBar = (  this->_buildBar( 1002,
                                             "icons/background.tga",
                                             osg::Vec2f ( _ll.x() + _barSize, _ll.y() + _barHeight ),
                                             osg::Vec2f(_ll.x() + _barLength , _ll.y() ),
                                             0.0f ) );
  this->_barBorder = (  this->_buildBar( 1001,
                                         "icons/barborder.tga",
                                         osg::Vec2f ( _ll.x() - _barBorderThickness, _ll.y() + _barHeight + _barBorderThickness ),
                                         osg::Vec2f( _ll.x() + _barLength + _barBorderThickness , _ll.y() -_barBorderThickness),
                                         _barBorderZOffset ) );
  this->_border = (  this->_buildBar( 1000,
                                      "icons/border.tga",
                                      osg::Vec2f ( _ll.x() - _borderPadding, _ll.y() + _borderHeight + _borderPadding ),
                                      osg::Vec2f(_ll.x() + _borderLength + _borderPadding, _ll.y() - _borderPadding),
                                      _borderZOffset ) );
  //this->_barText = ( "Loading file: test.txt " );
  this->_text = this->_drawTextAtPosition ( osg::Vec3f ( _ll.x(), _ll.y() + _barHeight + _barBorderThickness * 3.0f, _textZOffset ), this->_barText );
  //draw percentage on the right side of the progress bar
  //this->_percent = this->_drawTextAtPosition ( osg::Vec3f ( _barLength + _barBorderThickness * 2.0f ,_barHeight *.25f, _textZOffset ), this->_getPercentComplete() );
  //draw percentage at the midpoint of the progress bar
  this->_percent = this->_drawTextAtPosition ( osg::Vec3f ( _ll.x() + _barLength *0.45f , _ll.y() + _barHeight *.25f, _textZOffset ), this->_getPercentComplete() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the progress bar group with bar objects.  Set matrix transform options
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::_buildProgressBar()
{  
  //matrix transforms
 
  osg::ref_ptr< osg::MatrixTransform > matrixT ( new osg::MatrixTransform() );

  //matrixT->setMatrix( osg::Matrix::translate( _pos.x(), _pos.y(), _pos.z() ) );

  matrixT->addChild ( _progressBar.get() );
  matrixT->addChild ( _backgroundBar.get() );
  matrixT->addChild ( _barBorder.get() );
  matrixT->addChild ( _border.get() );
  matrixT->addChild ( _text.get() );
  matrixT->addChild ( _percent.get() );

  if( _isRelativeToAbsolute )
    matrixT->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );

  
  _pbarGroup->removeChildren ( 0, _pbarGroup->getNumChildren() ); 

  
  /*_pbarGroup->addChild ( backXform.get() );
  _pbarGroup->addChild ( pbarXform.get() );
  _pbarGroup->addChild ( barbdrXform.get() );*/

  _pbarGroup->addChild ( matrixT.release() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build an individual progress bar object (i.e. the bar, the border, the
//  background, etc. )
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ProgressBar::_buildBar( int render_level , std::string tex, const osg::Vec2f& ul, const osg::Vec2f& lr, float depth  )
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
  white->push_back ( osg::Vec4 ( 1.0f, 1.0f, 1.0f, 1.0f ) );

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

  osg::ref_ptr< osg::Vec2Array > uvcoords ( new osg::Vec2Array ( 4, texCoords ) );

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

