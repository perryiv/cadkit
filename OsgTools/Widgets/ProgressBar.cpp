
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Jeff Conner and Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/ProgressBar.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/ReadFile"
#include "osg/MatrixTransform"
#include "osg/Texture2D"
#include "osg/Image"
#include "osg/Geometry"
#include "osg/Geode"
#include "osgText/Font"
#include "osgText/Text"

using namespace OsgTools::Widgets;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ProgressBar, ProgressBar::BaseClass );

class ProgressBarAnimationCallback : public osg::NodeCallback
{
public:

  ProgressBarAnimationCallback( ProgressBar* bar, float start, float end, float step ) :
      _start ( start ),
      _end ( end ),
      _step ( step ),
      _bar ( bar )
        
    {
      
    }

    virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
    {
      osg::MatrixTransform* transform = dynamic_cast < osg::MatrixTransform* > ( node );    
      if( 0x0 != transform && _bar.valid() )
      {
        if(_bar->isAnimating())
        {
          if(_bar->isVisible())
          {
            if (_start < _end)
            {
              _start += _step;
              transform->setMatrix(osg::Matrix::scale(1.0f, _start, 1.0f));
      		    
            }
            else
              _bar->setAnimation(false);
          }
          else
          {
            if (_start > _end)
            {
              _start += _step;
              transform->setMatrix(osg::Matrix::scale(1.0f, _start, 1.0f));
            						
            }
            else
              _bar->setAnimation(false);
          }
        }
      }   
      traverse(node,nv);            
        
    }
    
protected:

  ProgressBar::RefPtr _bar;
    
  float				_start;
  float				_end;
  float				_step;

};


///////////////////////////////////////////////////////////////////////////////
//
//  Build a Quad.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail 
{
  osg::Node* buildQuad ( unsigned int render_level, const std::string& filename, const osg::Vec2f& ul, const osg::Vec2f& lr, float depth, osg::Geometry& geometry  )
  {
    std::string directory ( Usul::CommandLine::Arguments::instance().directory() + "/" + filename );

    osg::ref_ptr< osg::Geode > geode ( new osg::Geode() );

    osg::ref_ptr< osg::StateSet > stateset ( geometry.getOrCreateStateSet() );
    osg::ref_ptr< osg::Image > image = osgDB::readImageFile( directory );

    if ( image.get() )
    {
      
	    osg::ref_ptr< osg::Texture2D > texture ( new osg::Texture2D() ); 
	    texture->setImage ( image.get() );
	    stateset->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
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

    geometry.setTexCoordArray ( 0 , uvcoords.get() );

    geometry.setVertexArray ( vertices.get() );

    geometry.setColorArray ( white.get() );
    geometry.setColorBinding ( osg::Geometry::BIND_OVERALL );
          
    geometry.setNormalArray ( normal.get() );
    geometry.setNormalBinding ( osg::Geometry::BIND_OVERALL );

    geometry.addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );

    geode->addDrawable ( &geometry );

    return geode.release();
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBar::ProgressBar() :
BaseClass(),
_min ( 0.0 ),
_max ( 100.0 ),
_current ( 0.0 ),
_barSize ( 0.0f ),
_backSize ( 0.5f ),
_barBorderThickness ( 0.005f ),
_barBorderZOffset ( -0.0001f ),
_borderPadding ( 0.01f ),
_borderZOffset ( -0.0002f ),
_textZOffset ( 0.0005f ),
_animationStart ( 1.0f ),
_animationEnd ( 0.0f ), 
_animationStep ( -1 * ( 1.0f / 20.0f ) ),
_textFlag ( OsgTools::Widgets::ProgressBar::NORMAL ),
_isRelativeToAbsolute ( false ),
_isFinished ( false ),
_isVisible ( true ),
_isAnimating ( false ),
_ll ( osg::Vec2f ( 0.0f, 0.0f ) ),
_pos ( osg::Vec3f( 0.0f, 0.0f, 0.0f ) ),
_barText ( "" ),
_progressBar ( new osg::Node() ),
_backgroundBar ( new osg::Node() ),
_barBorder ( new osg::Node ),
_border ( new osg::Node() ),
_pbarGroup ( new osg::Group() ),
_barLH ( osg::Vec2f( 0.5f, 0.05f ) ),
_labelText ( new ThreadSafeText ),
_percentText ( new ThreadSafeText ),
_progressDrawable ( new UpdateProgress ),
_backgroundDrawable ( new UpdateProgress ),
_barBorderDrawable ( new UpdateProgress ),
_borderDrawable ( new UpdateProgress )
{
  _progressDrawable->setUseDisplayList ( false );
  _backgroundDrawable->setUseDisplayList ( false );
  _barBorderDrawable->setUseDisplayList ( false );
  _borderDrawable->setUseDisplayList ( false );

  this->reset();
	this->_buildProgressBarObject();
  this->showProgressBar();
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
//  Get the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ProgressBar::getProgressBar()
{
  return _pbarGroup.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec2f ProgressBar::getLowerLeft() 
{ 
  return osg::Vec2f( _pos.x(), _pos.y() ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset progress bar values to the starting parameters
//
///////////////////////////////////////////////////////////////////////////////

float ProgressBar::getLength()
{
  return _borderLH.x();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset progress bar values to the starting parameters
//
///////////////////////////////////////////////////////////////////////////////

float ProgressBar::getHeight()
{
  return _borderLH.y();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset progress bar values to the starting parameters
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::reset()
{
   _borderPos = osg::Vec3f ( _ll.x(), _ll.y(), _borderZOffset );

  _barBorderPos = osg::Vec3f ( _borderPos.x() + _borderPadding,
                               _borderPos.y() + _borderPadding,
                               _barBorderZOffset);

  _barPos = osg::Vec3f ( _barBorderPos.x() + _barBorderThickness,
                         _barBorderPos.y() + _barBorderThickness,
                         0.0f );

  _barBorderLH = osg::Vec2f ( _barBorderPos.x() + _barLH.x()  ,
                              _barBorderPos.y() + _barLH.y()   );

  _borderLH = osg::Vec2f ( ( _borderPos.x() + _barBorderLH.x() + _borderPadding * 2 ),
                           ( _borderPos.y() + _barBorderLH.y() + _borderPadding ) * 2 );

  _progressDrawable->setBounds ( osg::Vec2f ( _barPos.x(), _barPos.y() + _barLH.y() ),
                                 osg::Vec2f( _barPos.x() + _barSize , _barPos.y() ) );

  _backgroundDrawable->setBounds ( osg::Vec2f ( _barPos.x() + _barSize, _barPos.y() + _barLH.y() ),
                                   osg::Vec2f ( _barPos.x() + _barLH.x() , _barPos.y() ) );
  
  _barBorderDrawable->setBounds ( osg::Vec2f ( _barBorderPos.x(), _barBorderPos.y() + _barBorderLH.y() ),
                                  osg::Vec2f ( _barBorderPos.x() + _barBorderLH.x() , _barBorderPos.y() ) );

  _borderDrawable->setBounds ( osg::Vec2f ( _borderPos.x(), _borderPos.y() + _borderLH.y() ),
                               osg::Vec2f ( _borderPos.x() + _borderLH.x() , _borderPos.y() ) );
  
  _labelPos   = ( osg::Vec3f ( _barBorderPos.x(), _borderPos.y() + _borderLH.y() * .75, _textZOffset) );

  _percentPos = ( osg::Vec3f ( _barPos.x() + ( _barLH.x() * 0.45f ) ,
                               _barPos.y() + ( _barLH.y() * 0.25f ) ,
                               _textZOffset) );

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
  //USUL_TRACE_SCOPE;

  if(!_isVisible)
  {
    _animationStart = 0.0f;
    _animationEnd = 1.0f; 
    _animationStep = 1.0f / 20.0f;
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
  //USUL_TRACE_SCOPE;

  if(_isVisible)
  {
    _animationStart = 1.0f;
    _animationEnd = 0.0f; 
    _animationStep = -1 * (1.0f / 20.0f);
    _isAnimating = true;
    _isVisible = false;
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
  this->setMax ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::updateProgressBar ( unsigned int value )
{
  USUL_TRACE_SCOPE;
  this->setCurrent ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setStatusBarText ( const std::string &text, bool force )
{
  USUL_TRACE_SCOPE;
  this->setMessage ( text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rebuild the progress bar objects after changes have been made.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::updateProgressBar()
{
  if( _isVisible )
  {
    this->_buildProgressBar();
  }

  if( !_isVisible && _isAnimating )
  {
    this->_buildProgressBar();
  }

  if( !_isVisible && !_isAnimating )
    _emptyProgressBar();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Set value to true to start animation
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setAnimation ( bool value )
{
  _isAnimating = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the minimum value
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setMin( double min )
{
  _min = min;

  if( _current >= _max )
  {
    _current = _max;
    _isFinished = true;
  }
  if( _current < _min )
    _current = _min;

  _barSize = _barLH.x() * ( ( _current + _min ) / _max );

 _progressDrawable->setBounds ( osg::Vec2f ( _barPos.x(), _barPos.y() + _barLH.y() ),
                                osg::Vec2f( _barPos.x() + _barSize , _barPos.y() ) );

 _backgroundDrawable->setBounds ( osg::Vec2f ( _barPos.x() + _barSize, _barPos.y() + _barLH.y() ),
                                  osg::Vec2f ( _barPos.x() + _barLH.x() , _barPos.y() ) );

  _percentText->setText ( this->_getPercentComplete() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum value
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setMax( double max )
{
  _max = max;

  if( _current >= _max )
  {
    _current = _max;
    _isFinished = true;
  }
  if( _current < _min )
    _current = _min;

  _barSize = _barLH.x() * ( ( _current + _min ) / _max );

 _progressDrawable->setBounds ( osg::Vec2f ( _barPos.x(), _barPos.y() + _barLH.y() ),
                                osg::Vec2f( _barPos.x() + _barSize , _barPos.y() ) );

 _backgroundDrawable->setBounds ( osg::Vec2f ( _barPos.x() + _barSize, _barPos.y() + _barLH.y() ),
                                  osg::Vec2f ( _barPos.x() + _barLH.x() , _barPos.y() ) );
  _percentText->setText ( this->_getPercentComplete() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the hight of the progress bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setBarHeight ( float h )
{
  _barLH.y() = h;
 // _borderPadding = ( _barLH.y() + _barBorderThickness ) * ( _barLH.x() + _barBorderThickness) * .5;
  this->reset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the hight of the progress bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setBarLengthAndHeight ( const osg::Vec2f & lh )
{
  _barLH.set( lh.x(), lh.y() );
  this->reset();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the progress bar message
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setTextBehavior ( unsigned int flag )
{
  _textFlag = flag;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the progress bar message
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setMessage ( const std::string& s )
{
  
  if( _textFlag == OsgTools::Widgets::ProgressBar::WORD_WRAP && s.length() > 50 )
  {
    std::string head = s.substr( 0, 50 );
    std::string tail = s.substr( 50, s.length() );
    _labelText->setText ( head + "\n" + tail );
  }
  else
  {
    _barText = s;
    _labelText->setText ( s );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lower left corner of the progress bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setLowerLeft( const osg::Vec2f & ll )
{
 /* _ll.x() = ll.x();
  _ll.y() = ll.y();*/
  
  _pos.x() = ll.x();
  _pos.y() = ll.y();
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the length of the bar
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::setBarLength ( float l )
{
  _barLH.x() = l;
  
  this->reset();
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

  _barSize = _barLH.x() * ( ( _current + _min ) / _max );

  _progressDrawable->setBounds ( osg::Vec2f ( _barPos.x(), _barPos.y() + _barLH.y() ),
                                  osg::Vec2f( _barPos.x() + _barSize , _barPos.y() ) );

  _backgroundDrawable->setBounds ( osg::Vec2f ( _barPos.x() + _barSize, _barPos.y() + _barLH.y() ),
                                    osg::Vec2f ( _barPos.x() + _barLH.x() , _barPos.y() ) );
  
  _percentText->setText ( this->_getPercentComplete() );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all children from the compass group.  Called when compass is
//  "hidden"
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::_emptyProgressBar()
{
	_pbarGroup->removeChildren ( 0, _pbarGroup->getNumChildren() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw a text string <s> at position <p>
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  Helper::ThreadSafeText* drawTextAtPosition( const osg::Vec3f & p, const std::string & s, const osg::Vec4f& color, float size )
  { 
    osg::ref_ptr< osgText::Font > font ( osgText::readFontFile ( "fonts/arial.ttf" ) );
    osg::ref_ptr < Helper::ThreadSafeText > text ( new Helper::ThreadSafeText );

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
//  Return the percentage complete
//
///////////////////////////////////////////////////////////////////////////////

std::string ProgressBar::_getPercentComplete()
{
  char s[10];
 
  double percentage =  ( _current + _min ) / _max;

  percentage *= 100;
  sprintf( s , "%0.1lf%c" , percentage, '%' );

  return s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the bar objects
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::_buildProgressBarObject()
{
  // build bar objects.
  osg::ref_ptr< osg::Group > group ( new osg::Group() );
  float barPadding = _borderPadding + _barBorderThickness;
 

  
  this->_progressBar =  ( Detail::buildQuad( 1003,
                                           "icons/bar.tga",
                                           osg::Vec2f ( _barPos.x(), _barPos.y() + _barLH.y() ),
                                           osg::Vec2f ( _barPos.x() + _barSize , _barPos.y() ),
                                           0.0f, *_progressDrawable ) );

  this->_backgroundBar = (  Detail::buildQuad( 1002,
                                             "icons/background.tga",
                                             osg::Vec2f ( _barPos.x() + _barSize, _barPos.y() + _barLH.y() ),
                                             osg::Vec2f( _barPos.x() + _barLH.x() , _barPos.y() ),
                                             0.0f, *_backgroundDrawable ) );

  
    //osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry() );
    this->_barBorder = ( Detail::buildQuad( 1001,
                                         "icons/barborder.tga",
                                         osg::Vec2f ( _barBorderPos.x(), _barBorderPos.y() + _barBorderLH.y() ),
                                         osg::Vec2f( _barBorderPos.x() + _barBorderLH.x() , _barBorderPos.y() ),
                                         _barBorderZOffset, *_barBorderDrawable ) );
  

  
    //osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry() );
    this->_border = ( Detail::buildQuad( 1000,
                                      "icons/border.tga",
                                      osg::Vec2f ( _borderPos.x(), _borderPos.y() + _borderLH.y() ),
                                      osg::Vec2f ( _borderPos.x() + _borderLH.x() , _borderPos.y() ),
                                      _borderZOffset, *_borderDrawable ) );
  

  osg::Vec4f white ( osg::Vec4f ( 1.0f, 1.0f, 1.0f, 1.0f ) );
  osg::Vec4f black ( osg::Vec4f ( 0.0f, 0.0f, 0.0f, 1.0f ) );

  // Create a label above the progress bar.
  _labelText = Detail::drawTextAtPosition ( osg::Vec3f ( 0.0f, 0.0f, 0.0f ), this->_barText, white, 0.024 );
  _text = new osg::Geode;
  _text->addDrawable ( _labelText.get() );

  // Draw percentage at the midpoint of the progress bar.
  _percentText = Detail::drawTextAtPosition ( osg::Vec3f ( 0.0f, 0.0f, 0.0f ),
                                                           this->_getPercentComplete(),
                                                           white,
                                                           0.024 );
  _percent = new osg::Geode;
  _percent->addDrawable ( _percentText.get() );
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
  osg::ref_ptr < osg::MatrixTransform > anim ( new osg::MatrixTransform() );

  anim->setUpdateCallback ( new ProgressBarAnimationCallback ( this, _animationStart, _animationEnd, _animationStep ) );

  anim->addChild ( _progressBar.get() );
  anim->addChild ( _backgroundBar.get() );
  anim->addChild ( _barBorder.get() );
  anim->addChild ( _border.get() );

  osg::ref_ptr< osg::MatrixTransform > textPos ( new osg::MatrixTransform() );
  textPos->setMatrix( osg::Matrix::translate( _labelPos ) );
  textPos->addChild( _text.get() );
  anim->addChild ( textPos.get() );

  osg::ref_ptr< osg::MatrixTransform > perPos  ( new osg::MatrixTransform() );
  perPos->setMatrix( osg::Matrix::translate( _percentPos ) );
  perPos->addChild( _percent.get() );
  anim->addChild ( perPos.get() );

  if( _isRelativeToAbsolute )
    matrixT->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );

  matrixT->setMatrix( osg::Matrix::translate ( _pos ) );

  matrixT->addChild ( anim.get() );
  
  _pbarGroup->removeChildren ( 0, _pbarGroup->getNumChildren() ); 

  _pbarGroup->addChild ( matrixT.get() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear circular references.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBar::clear()
{
  _pbarGroup->removeChild ( 0 , _pbarGroup->getNumChildren() );
  _pbarGroup = 0x0;
}
