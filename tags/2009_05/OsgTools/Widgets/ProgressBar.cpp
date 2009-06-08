
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Jeff Conner and Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/ProgressBar.h"

#include "OsgTools/State/StateSet.h"

#include "Usul/App/Application.h"
#include "Usul/Trace/Trace.h"

#include "osg/MatrixTransform"
#include "osg/Texture2D"
#include "osg/Image"
#include "osg/Geometry"
#include "osg/Geode"

#include "osgDB/ReadFile"

#include "osgText/Font"
#include "osgText/Text"

using namespace OsgTools::Widgets;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ProgressBar, ProgressBar::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Animation Callback to show and hide the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

class ProgressBarAnimationCallback : public osg::NodeCallback
{
public:

  ProgressBarAnimationCallback( ProgressBar* bar ) :
      _bar ( bar )     
    { 
    }

    virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv )
    {
      osg::MatrixTransform* transform = dynamic_cast < osg::MatrixTransform* > ( node );    
      if( 0x0 != transform && _bar.valid() )
      {
        if( _bar->isAnimating() )
        {
          if( _bar->isVisible() )
          {
            if ( _bar->getAnimationCurrent() < _bar->getAnimationEnd() )
            {
              _bar->stepAnimation();
              transform->setMatrix ( osg::Matrix::scale ( 1.0f, _bar->getAnimationCurrent(), 1.0f ) );
      		    
            }
            else
            {
              _bar->setAnimation ( false );
            }
          }
          else
          {
            if ( _bar->getAnimationCurrent() > _bar->getAnimationEnd() )
            {
              _bar->stepAnimation();
              transform->setMatrix ( osg::Matrix::scale ( 1.0f, _bar->getAnimationCurrent(), 1.0f ) );
            						
            }
            else
            {
              _bar->setAnimation ( false );
            }
          }
        }
      }   
      traverse ( node,nv );            
        
    }
    
private:
  ProgressBar::RefPtr _bar;

};


///////////////////////////////////////////////////////////////////////////////
//
//  Build a Quad.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail 
{
  osg::Node* buildQuad ( unsigned int render_level, const std::string& filename, const osg::Vec2f& ul, const osg::Vec2f& lr, float depth, const osg::Vec4& color )
  {
    std::string directory ( Usul::App::Application::instance().iconDirectory() + "/" + filename );

    osg::ref_ptr< osg::Geode > geode ( new osg::Geode);
    osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );

    osg::ref_ptr< osg::StateSet > stateset ( geometry->getOrCreateStateSet() );
    osg::ref_ptr< osg::Image > image = osgDB::readImageFile( directory );

    if ( image.get() )
    {
	    osg::ref_ptr< osg::Texture2D > texture ( new osg::Texture2D() ); 
	    texture->setImage ( image.get() );
      texture->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT );
	    stateset->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
      
      osg::Vec2 texCoords[] =
      {
        osg::Vec2(0.0f, 0.0f),
        osg::Vec2(1.0f, 0.0f),
        osg::Vec2(1.0f, 1.0f),
        osg::Vec2(0.0f, 1.0f),            
      };
      
      osg::ref_ptr< osg::Vec2Array > uvcoords ( new osg::Vec2Array ( 4, texCoords ) );
      
      geometry->setTexCoordArray ( 0, uvcoords.get() );
    }

    osg::ref_ptr< osg::Vec4Array > white ( new osg::Vec4Array );
    white->push_back ( color );

    osg::ref_ptr< osg::Vec3Array > normal ( new osg::Vec3Array() );
    normal->push_back ( osg::Vec3 ( 0.0f, 0.0f, 1.0f ) );

    osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array() );
    vertices->reserve( 4 );
    vertices->push_back ( osg::Vec3f ( ul.x(), lr.y(), depth ) );
    vertices->push_back ( osg::Vec3f ( lr.x(), lr.y(), depth ) );
    vertices->push_back ( osg::Vec3f ( lr.x(), ul.y(), depth ) );
    vertices->push_back ( osg::Vec3f ( ul.x(), ul.y(), depth ) );

    geometry->setVertexArray ( vertices.get() );

    geometry->setColorArray ( white.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );
          
    geometry->setNormalArray ( normal.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );

    geometry->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );

    geode->getOrCreateStateSet()->setRenderBinDetails ( render_level, "RenderBin" );

    geode->addDrawable ( geometry.get() );

    return geode.release();
  }

}
///////////////////////////////////////////////////////////////////////////////
//
//  Draw a text string <s> at position <p>
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osgText::Text* drawTextAtPosition( const osg::Vec3f & p, const std::string & s, const osg::Vec4f& color, float size )
  { 
    osg::ref_ptr<osgText::Font> font ( osgText::readFontFile ( "fonts/arial.ttf" ) );
    osg::ref_ptr<osgText::Text> text ( new osgText::Text );

    text->setFont( font.get() );
    text->setColor( color );
    text->setCharacterSize( size );
    text->setPosition( p );
    text->setLayout( osgText::Text::LEFT_TO_RIGHT );
    text->setFontResolution ( 40, 40 );
    text->setText( s );

    return text.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////
 
ProgressBar::ProgressBar() : BaseClass(),
  _range   ( 0, 100 ),
  _value   ( 0 ),
  _barSize ( 0 ),
  _text    (),
  _size    ( 0.0f, 0.0f ),
  _padding ( 0.01f, 0.005f ),
  _pos     ( 0.0f, 0.0f, 0.0f ),
  _barPos ( 0, 0, 0 ),
  _barBorderPos ( 0, 0, 0 ),
  _borderPos ( 0, 0, 0 ),
  _barLH ( 0, 0 ),
  _barBorderLH ( 0, 0 ),
  _borderLH ( 0, 0 ),
  _percentPos ( 0, 0, 0 ),
  _labelPos ( 0, 0, 0 ),
  _animationCurEndStep ( 1, 1, .05 ),
  _root ( new osg::Group ),
  _dirty   ( true ),
  _isVisible ( false ),
  _isAnimating ( false ),
  _isFinished ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ProgressBar::~ProgressBar()
{
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
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if(!_isVisible)
  {
    this->_setupAnimation( 0.0f, 1.0f, 0.1f );
    _isAnimating = true;
    _isVisible = true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::hideProgressBar()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if(_isVisible)
  {
    this->_setupAnimation( 1.0f, 0.0f, -0.1f );
    _isAnimating = true;
    _isVisible = false;
    this->finished( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the total of progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setTotalProgressBar ( unsigned int value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _range.set ( 0, static_cast<double> ( value ) );
  _layout();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::updateProgressBar ( unsigned int value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->value ( static_cast<double> ( value ) );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setStatusBarText ( const std::string &text, bool force )
{
  USUL_TRACE_SCOPE;
  this->text ( text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset progress bar values to the starting parameters
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::finished( bool value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _isFinished = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset progress bar values to the starting parameters
//
///////////////////////////////////////////////////////////////////////////////

float ProgressBar::getLength()
{
  Guard guard ( this->mutex() );
  return _size[0];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset progress bar values to the starting parameters
//
///////////////////////////////////////////////////////////////////////////////

float ProgressBar::getHeight()
{
  Guard guard ( this->mutex() );
  return _size[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec2f ProgressBar::getLowerLeft() const 
{ 
  Guard guard ( this->mutex() );
  return Usul::Math::Vec2f ( _pos[0], _pos[1] ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lower left corner of the progress bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setLowerLeft( const Usul::Math::Vec2f & ll )
{
  Guard guard ( this->mutex() );
  _pos[0] = ll[0];
  _pos[1] = ll[1];
  this->_layout();
  this->_setDirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the length and height of the progress bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setBarLengthAndHeight ( const Usul::Math::Vec2f& lh )
{
  Guard guard ( this->mutex() );
  _barLH.set( lh[0], lh[1] );
  _layout();
  this->_setDirty( true );
  
}


///////////////////////////////////////////////////////////////////////////////
//
// Return true if the progress is being animated
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setAnimation ( bool value )
{
  Guard guard ( this->mutex() );
  _isAnimating = value;
}


///////////////////////////////////////////////////////////////////////////////
//
// Return true if the progress is being animated
//
///////////////////////////////////////////////////////////////////////////////

bool ProgressBar::isAnimating() 
{ 
  Guard guard ( this->mutex() );
  return _isAnimating; 
}


///////////////////////////////////////////////////////////////////////////////
//
// Return true if the progress bar is being shown
//
///////////////////////////////////////////////////////////////////////////////

bool ProgressBar::isVisible() 
{ 
  Guard guard ( this->mutex() );
  return _isVisible; 
}


///////////////////////////////////////////////////////////////////////////////
//
// Return true if the progress bar is 100% complete
//
///////////////////////////////////////////////////////////////////////////////

bool ProgressBar::isFinished() 
{ 
  Guard guard ( this->mutex() );
  return _isFinished; 
}


///////////////////////////////////////////////////////////////////////////////
//
// Return the animation step value
//
///////////////////////////////////////////////////////////////////////////////

float ProgressBar::getAnimationStep () 
{ 
  Guard guard ( this->mutex() );
  return _animationCurEndStep[2]; 
}


///////////////////////////////////////////////////////////////////////////////
//
// Return the animation end value
//
///////////////////////////////////////////////////////////////////////////////

float ProgressBar::getAnimationEnd () 
{ 
  Guard guard ( this->mutex() );
  return _animationCurEndStep[1];
}


///////////////////////////////////////////////////////////////////////////////
//
// Return the animation current value
//
///////////////////////////////////////////////////////////////////////////////

float ProgressBar::getAnimationCurrent () 
{ 
  Guard guard ( this->mutex() );
  return _animationCurEndStep[0];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::text ( const std::string &t )
{
  Guard guard ( this->mutex() );
  _text = t;
  this->_setDirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the range.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec2d ProgressBar::range() const
{
  Guard guard ( this->mutex() );
  return _range;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::value ( double v )
{
  Guard guard ( this->mutex() );

  Usul::Math::Vec2d range ( this->range() );

  if( v >= range[1] )
  {
    v = range[1];
    //_isFinished = true;
  }
  if( v < range[0] )
    v = range[0];

  _barSize = _barLH[0] * ( ( v + range[0] ) / range[1] );

  _value = v;
  this->_layout();
  this->_setDirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value
//
///////////////////////////////////////////////////////////////////////////////

double ProgressBar::value() const
{
  Guard guard ( this->mutex() );
  return _value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec2f ProgressBar::size() const
{
  Guard guard ( this->mutex() );
  return _size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get eh amount of padding
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec2f ProgressBar::padding() const
{
  Guard guard ( this->mutex() );
  return _padding;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we dirty?
//
///////////////////////////////////////////////////////////////////////////////

std::string ProgressBar::text() const
{
  Guard guard ( this->mutex() );
  return _text;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * ProgressBar::buildScene()
{
  if( _isVisible || ( !_isVisible && _isAnimating ) )
    return this->_buildScene();
  else
    return this->_emptyScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Step through 1 frame of animation
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::stepAnimation()
{
  Guard guard ( this->mutex() );
  _animationCurEndStep[0] += _animationCurEndStep[2];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear circular references.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::clear()
{
  _root->removeChild ( 0, _root->getNumChildren() );
  _root = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Setup animation parameters.  Called from show and hide progressbar to 
//  initialize the start and ending values.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::_setupAnimation( float cur, float end, float step )
{
  Guard guard ( this->mutex() );
  _animationCurEndStep[0] = cur;
  _animationCurEndStep[1] = end;
  _animationCurEndStep[2] = step;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset progress bar values to the starting parameters
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::_layout()
{
  _borderPos = Usul::Math::Vec3f ( 0, 0, 0.0f );
  
  _barBorderPos = Usul::Math::Vec3f( this->_borderPos[0] + this->_padding[0],
                                     this->_borderPos[1] + this->_padding[0],
                                     0.0f );

  _barPos = Usul::Math::Vec3f ( this->_barBorderPos[0] + this->_padding[1],
                                this->_barBorderPos[1] + this->_padding[1],
                                0.0f );

  _barBorderLH = Usul::Math::Vec2f (  this->_barLH[0] + ( _padding[1] * 2 ),
                                      this->_barLH[1] + ( _padding[1] * 2 ) );

  _size = Usul::Math::Vec2f ( ( this->_barBorderLH[0] + this->_padding[0] * 2 ),
                              ( this->_barBorderLH[1] + this->_padding[0] * 2 ) * 2 );

  _labelPos   = ( Usul::Math::Vec3f ( this->_barBorderPos[0], this->_borderPos[1] + this->_size[1] * .75, 0.0f ) );

  _percentPos = ( Usul::Math::Vec3f ( this->_barPos[0] + ( this->_barLH[0] * 0.45f ) ,
                                      this->_barPos[1] + ( this->_barLH[1] * 0.25f ) ,
                                      0.0f ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Are we dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool ProgressBar::_isDirty() const
{
  Guard guard ( this->mutex() );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::_setDirty ( bool d )
{
  Guard guard ( this->mutex() );
  _dirty = d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the percentage complete
//
///////////////////////////////////////////////////////////////////////////////

std::string ProgressBar::_getPercentComplete()
{
  Guard guard ( this->mutex() );

  char s[10];
 
  double percentage =  ( _value + _range[0] ) / _range[1];

  percentage *= 100;
  sprintf( s, "%0.1lf%c" , percentage, '%' );

  return s;
}




///////////////////////////////////////////////////////////////////////////////
//
//  Empty the scene.  Called when progress bar is not visible
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * ProgressBar::_emptyScene()
{
  Guard guard ( this->mutex() );
  _root->removeChildren( 0, _root->getNumChildren() );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.  Called when progress bar is visible
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * ProgressBar::_buildScene()
{
  if ( false == this->_isDirty() )
  {
    Guard guard ( this->mutex() );
    return _root.get();
  }
  else
  {
    Guard guard ( this->mutex() );
    
    osg::ref_ptr<osg::MatrixTransform> anim ( new osg::MatrixTransform );

    // build scene..
    
    anim->getOrCreateStateSet()->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    
    anim->addChild ( Detail::buildQuad ( 1000, 
                                        "background.tga", 
                                        osg::Vec2f ( _barPos[0] + _barSize, _barPos[1] + _barLH[1] ), 
                                        osg::Vec2f( _barPos[0] + _barLH[0], _barPos[1] ), 
                                        0.0f, osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) ) );

    anim->addChild ( Detail::buildQuad( 1001,
                                       "border.tga",
                                       osg::Vec2f ( _borderPos[0], _borderPos[1] + _size[1] ),
                                       osg::Vec2f ( _borderPos[0] + _size[0], _borderPos[1] ),
                                       _borderPos[2], osg::Vec4 ( 0.58, 0.58, 0.58, 1.0 ) ) );

    anim->addChild ( Detail::buildQuad( 1003,
                                        "barborder.tga",
                                        osg::Vec2f ( _barBorderPos[0], _barBorderPos[1] + _barBorderLH[1] ),
                                        osg::Vec2f( _barBorderPos[0] + _barBorderLH[0], _barBorderPos[1] ),
                                        _barBorderPos[2], osg::Vec4 ( 0.75, 0.75, 0.75, 1.0 ) ) );

    anim->addChild ( Detail::buildQuad( 1004,
                                        "bar.tga",
                                        osg::Vec2f ( _barPos[0], _barPos[1] + _barLH[1] ),
                                        osg::Vec2f ( _barPos[0] + _barSize, _barPos[1] ),
                                        0.0f, osg::Vec4 ( 0.0, 0.0, 1.0, 1.0 ) ) );

    osg::Vec4f white ( osg::Vec4f ( 1.0f, 1.0f, 1.0f, 1.0f ) );

    // Create a label above the progress bar.
    {
      osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
      geode->addDrawable ( Detail::drawTextAtPosition 
                            ( osg::Vec3f ( _labelPos[0], _labelPos[1], _labelPos[2] ),
                              this->_text,
                              white,
                              0.3 * _size[1] ) 
                          );
      geode->getOrCreateStateSet()->setRenderBinDetails ( 1005, "RenderBin" );
      anim->addChild ( geode.get() );
    }

    // Draw percentage at the midpoint of the progress bar.
    { 
      osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
      geode->addDrawable ( Detail::drawTextAtPosition 
                            ( osg::Vec3f ( _percentPos[0], _percentPos[1], _percentPos[2] ),
                              this->_getPercentComplete(),
                              white,
                              0.3 * _size[1] ) 
                          );
      geode->getOrCreateStateSet()->setRenderBinDetails ( 1006, "RenderBin" );
      anim->addChild ( geode.get() );
    }
    
    anim->setUpdateCallback ( new ProgressBarAnimationCallback ( this ) );

    osg::ref_ptr<osg::MatrixTransform> matrix ( new osg::MatrixTransform );
    matrix->setMatrix( osg::Matrix::translate ( osg::Vec3f( _pos[0], _pos[1], _pos[2] ) ) );
    matrix->addChild( anim.get() );

    osg::ref_ptr<osg::Group> root ( new osg::Group );
    root->addChild( matrix.get() );
    
    // Turn off lighting
    OsgTools::State::StateSet::setLighting ( root.get(), false );

    {
      Guard guard ( this->mutex() );
      _root = root;
    }

    this->_setDirty ( false );

    return root.release();
  }
}
