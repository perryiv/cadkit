
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "TFView.h"

#include "FoxTools/Headers/App.h" // for FX::FXEvent
#include "FoxTools/Errors/ErrorChecker.h"
#include "FoxTools/Functions/Mouse.h"

#include "OsgTools/Render/EventAdapter.h"
#include "OsgTools/State/StateSet.h"

#include "OsgTools/Draggers/Callback.h"
#include "OsgTools/Draggers/Translate1.h"
#include "OsgTools/Draggers/Translate2.h"

#include "Usul/Math/Absolute.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Group"
#include "osg/MatrixTransform"

#include "osgUtil/IntersectVisitor"

#include <algorithm>

using namespace Helios::Views;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( TFView ) TFViewMap[] =
{
  // Message type,                       id,   handler.
  FXMAPFUNC ( FX::SEL_PAINT,             0,    TFView::onPaint             ),
  FXMAPFUNC ( FX::SEL_FOCUSIN,           0,    TFView::onFocusIn           ),
  FXMAPFUNC ( FX::SEL_CONFIGURE,         0,    TFView::onResize            ),  
  FXMAPFUNC ( FX::SEL_LEFTBUTTONPRESS,   0,    TFView::onLeftBtnPress      ),
  FXMAPFUNC ( FX::SEL_LEFTBUTTONRELEASE, 0,    TFView::onLeftBtnRelease    ),
  FXMAPFUNC ( FX::SEL_RIGHTBUTTONPRESS,  0,    TFView::onRightBtnPress     ),
  FXMAPFUNC ( FX::SEL_MOTION,            0,    TFView::onMotion            ),
};

// This class, base class, array of message handlers, size of the array.
FOX_TOOLS_IMPLEMENT ( TFView, BaseClass, TFViewMap, ARRAYNUMBER ( TFViewMap ) );


namespace Detail
{
  struct Sort
  {
    template < class Vertex >
    bool operator () ( const Vertex& v0, const Vertex& v1 ) const
    {
      return v0[0] < v1[0];
    }
  };

  class Callback : public OsgTools::Draggers::Callback
  {
  public:

    Callback ( Usul::Math::Vec2f& orig, osg::Vec3Array * vertices, unsigned int index, float heightNorm, float widthNorm ) :
    _original ( orig ),
    _value ( orig ),
    _originalVec3 ( vertices->at( index ) ),
    _vertices( vertices ),
    _index ( index ),
    _heightNorm ( heightNorm ),
    _widthNorm ( widthNorm )
    {
    }

    // Execute the callback.
    virtual void operator() ( OsgTools::Draggers::Dragger &d )
    {
      osg::Matrix m ( d.getMatrix() );

      osg::Vec3 t ( m.getTrans() );

      float x ( _original[0] );
      float y ( _original[1] );

      y *= _heightNorm;
      x *= _widthNorm;

      _vertices->at( _index ).x() = x + t.x();
      _vertices->at( _index ).y() = y + t.y();

      float tx ( t.x() );
      float ty ( t.y() );

      ty /= _heightNorm;
      tx /= _widthNorm;

      _value[0] = _original[0] + tx;
      _value[1] = _original[1] + ty;

      // Don't need to check the first of last since they are 1D draggers
      //if ( _index != 0 || _index != _vertices->size() - 1 && _vertices->size() > 2 )
      {
        //osg::Vec3 left ( _vertices->at( _index - 1 ) );
        //osg::Vec3 middle ( _vertices->at ( _index ) );
        //osg::Vec3 right ( _vertices->at ( _index + 1 ) );

        // Check x values
        if ( _value[0] < 0.0 )
        {
          _value[0] = 0.0f;
          _vertices->at( _index ).x() = 0.0f;
          t.x() = -_originalVec3.x();
          m.setTrans( t );
          d.setMatrix ( m );
        }
        else if ( _value[0] > 255.0 )
        {
          _value[0] = 255;

          float xLoc ( 255 * _widthNorm );

          _vertices->at( _index ).x() = xLoc;
          t.x() = xLoc - _originalVec3.x();
          m.setTrans( t );
          d.setMatrix ( m );
        }
      }

      // Check y
      if( _value[1] < 0.0 )
      {
        _value[1] = 0.0f;

        _vertices->at( _index ).y() = 0.0;
        t.y() = -_originalVec3.y();
        m.setTrans( t );
        d.setMatrix ( m );
      }
      else if ( _value[1] > 1.0 )
      {
        _value[1] = 1.0f;

        float yLoc ( _heightNorm );

        _vertices->at( _index ).y() = yLoc;
        t.y() = yLoc - _originalVec3.y();
        m.setTrans( t );
        d.setMatrix ( m );
      }

    }

  private:
    Usul::Math::Vec2f _original;
    Usul::Math::Vec2f& _value;
    osg::Vec3 _originalVec3;
    osg::ref_ptr< osg::Vec3Array > _vertices;
    unsigned int _index;
    float _heightNorm;
    float _widthNorm;
  };
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TFView::TFView( Usul::Interfaces::IDocument* document, FX::FXComposite *parent, FX::FXGLVisual *visual ) :
  BaseClass ( parent, visual ),
  _document ( document ),
  _paddingPercent ( 0.05f ),
  _graphPercent ( 0.9f ),
  _viewer( new osgUtil::SceneView ),
  _controlPoints()
{
  _viewer->setDefaults();
  _viewer->setClearColor( osg::Vec4 ( 0.9, 1.0, 0.5, 1.0 ) );

  _document->addView( this );

  _controlPoints.push_back ( ControlPoint ( 0.0, 1.0 ) );
  _controlPoints.push_back ( ControlPoint ( 255.0, 1.0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deconstructor.
//
///////////////////////////////////////////////////////////////////////////////

TFView::~TFView()
{
  _document->removeView ( this );

  Usul::Interfaces::IViewer::ValidQueryPtr me ( this );

  // If we are the active view, set the active view to null.
  if( me.get() == _document->activeView() )
    _document->activeView( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.  Usul::Interfaces::IViewer
//
///////////////////////////////////////////////////////////////////////////////

void TFView::render()
{
  this->makeCurrent();

  _viewer->cull();
  _viewer->draw();

  this->swapBuffers();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Clear the scene.  Usul::Interfaces::IViewer
//
///////////////////////////////////////////////////////////////////////////////

void TFView::clearScene()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the x position.  Usul::Interfaces::IViewer
//
///////////////////////////////////////////////////////////////////////////////

int TFView::x()
{
  return this->getX();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the y position.  Usul::Interfaces::IViewer
//
///////////////////////////////////////////////////////////////////////////////

int TFView::y() 
{
  return this->getY();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the height.  Usul::Interfaces::IViewer
//
///////////////////////////////////////////////////////////////////////////////

 int TFView::height() 
{
  return static_cast<unsigned int> ( Usul::Math::absolute ( this->getHeight() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the width.  Usul::Interfaces::IViewer
//
///////////////////////////////////////////////////////////////////////////////

 int TFView::width() 
{
  return static_cast<unsigned int> ( Usul::Math::absolute ( this->getWidth() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the given message.  Usul::Interfaces::IViewer
//
///////////////////////////////////////////////////////////////////////////////

void TFView::handleMessage ( unsigned short message )
{
  this->handle ( 0x0, FXSEL ( FX::SEL_COMMAND, message ), 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void TFView::create()
{
  // Call the base class's function first.
  BaseClass::create();

  // Pop this window to the front.
  this->setFocus();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the given interface
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* TFView::queryInterface( unsigned long iid )
{
  switch( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IViewer::IID:
    return static_cast< Usul::Interfaces::IViewer* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

long TFView::onResize( FX::FXObject *object, FX::FXSelector selector, void * callData )
{
  FX::FXEvent *event = (FX::FXEvent *) callData;

  // Resize the window.
  unsigned int width  ( static_cast < unsigned int > ( event->rect.w ) );
  unsigned int height ( static_cast < unsigned int > ( event->rect.h ) );
  
  if ( width > 1 || height > 1 )
  {
    if ( _viewer.valid() )
    {
      _viewer->setViewport ( 0, 0, (int) width, (int) height );
      _viewer->setProjectionMatrixAsOrtho ( 0, width, 0, height, -10, 10 );

      this->build();
    }
  }

  // Call the base class's function.
  return BaseClass::onConfigure ( object, selector, callData );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Paint event.
//
///////////////////////////////////////////////////////////////////////////////

long TFView::onPaint ( FX::FXObject *, FX::FXSelector, void * )
{
  this->render();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

long TFView::onFocusIn ( FX::FXObject *, FX::FXSelector, void * )
{
  this->render();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void TFView::ref()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void TFView::unref( bool )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the left mouse button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

long TFView::onLeftBtnPress ( FX::FXObject *object, FX::FXSelector selector, void *callData )
{
  // Get the event.
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != callData );
  FX::FXEvent *event = (FX::FXEvent *) callData;

  // Handle the events. Make sure you pick before you drag.
  this->_handlePicking  ( *event );
  this->_handleDragging ( *event, Dragger::START );
  

  // Call the base class's function.
  return BaseClass::onLeftBtnPress ( object, selector, callData );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the left mouse button is released.
//
///////////////////////////////////////////////////////////////////////////////

long TFView::onLeftBtnRelease ( FX::FXObject *object, FX::FXSelector selector, void *callData )
{
  // Get the event.
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != callData );
  FX::FXEvent *event = (FX::FXEvent *) callData;

  // Handle the events. Make sure you pick before you drag.
  this->_handlePicking  ( *event );
  this->_handleDragging ( *event, Dragger::FINISH );

  // Make sure.
  Dragger::dragging ( 0x0, osg::Matrixd::identity() );

  // Call the base class's function.
  return BaseClass::onLeftBtnRelease ( object, selector, callData );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the mouse is moved over the window.
//
///////////////////////////////////////////////////////////////////////////////

long TFView::onMotion ( FX::FXObject *object, FX::FXSelector selector, void *callData )
{
  // Get the event.
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != callData );
  FX::FXEvent *event = (FX::FXEvent *) callData;

  // See if any mouses button are down.
  unsigned int mouse ( FoxTools::Functions::mouseState ( event ) );

  // Handle the events. Make sure you pick before you drag.
  this->_handlePicking    ( *event );
  this->_handleDragging   ( *event, Dragger::MOVE );

  // Call the base class's function.
  return BaseClass::onMotion ( object, selector, callData );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

long TFView::onRightBtnPress     ( FX::FXObject *object, FX::FXSelector, void *callData )
{
  FX::FXEvent *event = (FX::FXEvent *) callData;

  float x ( event->click_x );
  float y ( event->click_y );

  y = this->getHeight() - y;

  const float xOffset ( this->getWidth() * _paddingPercent );
  const float yOffset ( this->getHeight() * _paddingPercent );
  const float heightNorm ( ( this->getHeight() * _graphPercent ) );
  const float widthNorm ( ( (float) this->getWidth() * _graphPercent ) / 255 );

  
  y -= yOffset;
  x -= xOffset;
  
  y /= heightNorm;
  x /= widthNorm;

  // Check x and y values to make sure they are within proper ranges
  if ( y < 0.0 )
    y = 0.0f;
  else if ( y > 1.0 )
    y = 1.0f;

  if( x < 0.0 )
    x = 0.0f;
  else if ( x > 255.0 )
    x = 255.0f;

  _controlPoints.push_back( ControlPoint ( x, y ) );

  std::sort ( _controlPoints.begin(), _controlPoints.end(), Detail::Sort() );

  this->build();
  this->render();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void TFView::build()
{
  // Clear the existing scene.
  _viewer->setSceneData( 0x0 );

  osg::ref_ptr< osg::MatrixTransform > root  ( new osg::MatrixTransform );

  const float xOffset ( this->getWidth() * _paddingPercent );
  const float yOffset ( this->getHeight() * _paddingPercent );

  root->setMatrix( osg::Matrix::translate( xOffset, yOffset, 0.0 ) );

  // Geode for the axes.
  osg::ref_ptr< osg::Geode > axes ( new osg::Geode );
  root->addChild( axes.get() );

  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );

  colors->push_back ( osg::Vec4 ( 0.0f, 0.0f, 1.0f, 1.0 ) );

  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );

  OsgTools::State::StateSet::setLineWidth ( axes.get(), 4.0f );

  // y-axes
  vertices->push_back ( osg::Vec3 ( 0, 0, -2.0f ) );
  vertices->push_back ( osg::Vec3 ( 0, ( this->getHeight() * _graphPercent ), -2.0f ) );

  // x-axis
  vertices->push_back ( osg::Vec3 ( 0, 0, -2.0f ) );
  vertices->push_back ( osg::Vec3 ( ( this->getWidth() * _graphPercent ), 0, -2.0f ) );

  geometry->setVertexArray( vertices.get() );

  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, vertices->size() ) );
  
  axes->addDrawable( geometry.get() );

  osg::ref_ptr< osg::Geode > circle ( new osg::Geode );
  osg::ref_ptr< osg::Geode > draggedCircle ( new osg::Geode );
  OsgTools::State::StateSet::setLineWidth ( circle.get(), 1.0f );

  {
    osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

    osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );

    colors->push_back ( osg::Vec4 ( 1.0f, 1.0f, 1.0f, 1.0 ) );

    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE_SET );

    osg::ref_ptr< osg::Vec3Array > outline ( new osg::Vec3Array );

    const float radius ( 0.01 * this->getWidth() );

    for ( float theta = 0; theta < 360; theta += 5.0 )
    {
      float x = radius * ::cos( osg::DegreesToRadians ( theta ) );
      float y = radius * ::sin ( osg::DegreesToRadians  ( theta ) );

      vertices->push_back ( osg::Vec3 ( x, y, -1.0 ) );
      outline->push_back ( osg::Vec3 ( x, y, 0.0 ) );
    }

    geometry->setVertexArray( vertices.get() );
    geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::POLYGON, 0, vertices->size() ) );

    circle->addDrawable( geometry.get() );
    draggedCircle->addDrawable( geometry.get() );

    {

      osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

      osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );

      colors->push_back ( osg::Vec4 ( 0.52f, 0.51f, 0.73f, 1.0 ) );

      geometry->setColorArray ( colors.get() );
      geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE_SET );
      geometry->setVertexArray( outline.get() );
      geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_LOOP, 0, vertices->size() ) );

      circle->addDrawable( geometry.get() );

      osg::ref_ptr< osg::Geometry > draggedGeom ( new osg::Geometry );
      osg::ref_ptr < osg::Vec4Array > draggedColors ( new osg::Vec4Array );

      draggedColors->push_back ( osg::Vec4 ( 0.0f, 1.0f, 0.0f, 1.0f ) );

      draggedGeom->setColorArray ( draggedColors.get() );
      draggedGeom->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE_SET );
      draggedGeom->setVertexArray( outline.get() );
      draggedGeom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_LOOP, 0, vertices->size() ) );
      draggedCircle->addDrawable( draggedGeom.get() );
    }
  }

  const float heightNorm ( ( this->getHeight() * _graphPercent ) );
  const float widthNorm ( ( (float) this->getWidth() * _graphPercent ) / 255 );

  osg::ref_ptr< osg::Geode > lines ( new osg::Geode );
  root->addChild( lines.get() );

  {
    osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
    geometry->setUseDisplayList( false );
    osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );

    colors->push_back ( osg::Vec4 ( 1.0f, 0.25f, 0.0f, 1.0 ) );

    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );

    OsgTools::State::StateSet::setLineWidth ( axes.get(), 1.0f );

    for( ControlPoints::iterator iter = _controlPoints.begin(); iter != _controlPoints.end(); ++iter )
    {
      float x ( (*iter)[0] );
      float y ( (*iter)[1] );

      y = y * heightNorm;
      x = x * widthNorm;

      vertices->push_back( osg::Vec3 ( x, y, -2.0 ) );

      
      osg::ref_ptr< OsgTools::Draggers::Dragger > dragger ( new OsgTools::Draggers::Translate2 ( ) );

      // If it's the first or last point, make sure it can only be dragged in the y direction.
      if( iter == _controlPoints.begin() || iter == _controlPoints.end() - 1 )
        dragger = new OsgTools::Draggers::Translate1( OsgTools::Draggers::Translate1::Y );

      osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform );
      mt->setMatrix( osg::Matrix::translate( x,y, 0.0 ) );

      mt->addChild( circle.get() );

      osg::ref_ptr< osg::MatrixTransform > draggedmt ( new osg::MatrixTransform );
      draggedmt->setMatrix( osg::Matrix::translate( x,y, 0.0 ) );

      draggedmt->addChild( draggedCircle.get() );


      //dragger->addChild( mt.get() );
      dragger->geometry( Dragger::NORMAL, mt.get() );
      dragger->geometry( Dragger::DRAGGING, draggedmt.get() );

      osg::ref_ptr< Detail::Callback > callback ( 
        new Detail::Callback( *iter, vertices.get(), iter-_controlPoints.begin(), heightNorm, widthNorm  ) );

      dragger->callback ( Dragger::MOVE, callback.get() );
      dragger->callback ( Dragger::FINISH, callback.get() );

      root->addChild( dragger.get() );
    }

    geometry->setVertexArray( vertices.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );

    lines->addDrawable( geometry.get() );
  }

  osg::ref_ptr< osg::Geode > background ( new osg::Geode );
  root->addChild ( background.get() );

  {
    osg::ref_ptr< osg::StateSet > stateset ( background->getOrCreateStateSet() );
    stateset->setMode( GL_BLEND, osg::StateAttribute::ON);
    //stateset->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    //stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
    osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );


    vertices->push_back ( osg::Vec3 ( 0.0, 0.0, -3.0 ) );
    colors->push_back ( osg::Vec4 ( 0.0, 0.0, 0.0, 0.0 ) );

    vertices->push_back ( osg::Vec3 ( 255 * widthNorm, 0.0, -3.0 ) );
    colors->push_back ( osg::Vec4 ( 1.0, 1.0, 1.0, 0.0 ) );

    vertices->push_back ( osg::Vec3 ( 255 * widthNorm, heightNorm, -3.0 ) );
    colors->push_back ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );

    vertices->push_back ( osg::Vec3 ( 0.0, heightNorm, -3.0 ) );
    colors->push_back ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );

    geometry->setVertexArray ( vertices.get() );
    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );

    geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::POLYGON, 0, vertices->size() ) );

    background->addDrawable ( geometry.get() );

  }


  _viewer->setSceneData( root.get( ) );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Handle the picking event.
//
///////////////////////////////////////////////////////////////////////////////

void TFView::_handlePicking ( const FX::FXEvent &event )
{
  // Intersect the scene.
  osgUtil::Hit hit;
  if ( !this->_intersect ( event, _viewer->getSceneData(), hit ) )
    return;

  // Left mouse button was pressed...
  if ( FX::SEL_LEFTBUTTONPRESS == event.type )
  {
    // Set the dragger being dragged, if there is one.
    this->_findDragger ( hit );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle dragging.
//
///////////////////////////////////////////////////////////////////////////////

void TFView::_handleDragging ( const FX::FXEvent &event, Dragger::Command command )
{
  {
    // Lock the dragger.
    Dragger::Guard guard ( Dragger::dragging().mutex() );

    // Make sure we are in picking mode, and there is a dragger.
    if ( 0x0 == Dragger::dragging().value().dragger.get() )
      return;

    // Calculate the two points for our line-segment.
    osg::Vec3 pt0, pt1;
    if ( !this->_lineSegment ( event, pt0, pt1 ) )
      return;

    // Have the dragger execute the command.
    Dragger::dragging().value().dragger->execute ( command, pt0, pt1 );
  }

  // Redraw.
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the deepest dragger if there is one (they may be nested) and set 
//  it as the dragger being dragged.
//
///////////////////////////////////////////////////////////////////////////////

void TFView::_findDragger ( const osgUtil::Hit &hit )
{
  // Initialize.
  Dragger *dragger = 0x0;
  osg::NodePath path ( hit._nodePath );

  // Loop through the nodes.
  for ( osg::NodePath::reverse_iterator i = path.rbegin(); i != path.rend(); ++i )
  {
    // See if it is a dragger.
    dragger = dynamic_cast < Dragger * > ( *i );
    if ( dragger )
      break;
  }

  // If we did not find a dragger...
  if ( 0x0 == dragger )
  {
    Dragger::dragging ( 0x0, osg::Matrixd::identity() );
    return;
  }

  // We found a dragger. Loop again from the root of the path.
  osg::Matrixd cumulative ( osg::Matrixd::identity() );
  for ( osg::NodePath::const_iterator i = path.begin(); i != path.end(); ++i )
  {
    // Stop at our dragger.
    if ( ( *i ) == static_cast < osg::Node * > ( dragger ) )
      break;

    // If it is a transform then accumulate.
    osg::MatrixTransform *mt = dynamic_cast < osg::MatrixTransform * > ( *i );
    if ( mt )
      cumulative = mt->getMatrix() * cumulative;
  }

  // Set the dragging data.
  Dragger::dragging ( dragger, cumulative );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line segment into the scene that goes through the 2D coordinate.
//
///////////////////////////////////////////////////////////////////////////////

bool TFView::_lineSegment ( const FX::FXEvent &event, osg::Vec3 &pt0, osg::Vec3 &pt1 )
{
  // Declare the event adapter.
  OsgTools::Render::EventAdapter::Ptr ea ( new OsgTools::Render::EventAdapter );
  ea->setWindowSize ( Usul::Math::Vec2ui( this->getWidth(), this->getHeight() ) );
  ea->setMouse ( Usul::Math::Vec2f( event.win_x, this->getHeight() - event.win_y ) );

  // Get the necessary coordinates.
  float x ( ea->getXnormalized() );
  float y ( ea->getYnormalized() );

  // Get the matrix.
  osg::Matrix M ( _viewer->getViewMatrix() );
  osg::Matrix P ( _viewer->getProjectionMatrix() );
  osg::Matrix MP ( M * P );
  osg::Matrix IMP ( osg::Matrix::inverse ( MP ) );

  // Calculate the two points for our line-segment.
  pt0 = osg::Vec3 ( x, y, -1 ) * IMP;
  pt1 = osg::Vec3 ( x, y,  1 ) * IMP;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the scene.
//
///////////////////////////////////////////////////////////////////////////////

bool TFView::_intersect ( const FX::FXEvent &event, osg::Node *scene, osgUtil::Hit &hit )
{
  // Calculate the two points for our line-segment.
  osg::Vec3 pt0, pt1;
  if ( !this->_lineSegment ( event, pt0, pt1 ) )
    return false;

  // Declare the pick-visitor.
  typedef osgUtil::IntersectVisitor Visitor;
  osg::ref_ptr<Visitor> visitor ( new Visitor );

  // Make the line segment.
  osg::ref_ptr<osg::LineSegment> ls ( new osg::LineSegment ( pt0, pt1 ) );
  visitor->addLineSegment ( ls.get() );

  // Intersect the scene.
  typedef osgUtil::IntersectVisitor::HitList HitList;
  scene->accept ( *visitor );

  // See if there was an intersection.
  if ( !visitor->hits() )
    return false;

  // Get the hit-list for our line-segment.
  const HitList &hits = visitor->getHitList ( ls.get() );
  if ( hits.empty() )
    return false;

  // Set the hit.
  hit = hits.front();

  // It worked.
  return true;
}

