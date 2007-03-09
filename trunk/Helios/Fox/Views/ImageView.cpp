
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ImageView.h"

#include "FoxTools/Headers/Image.h"
#include "FoxTools/Headers/ColorDialog.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/Vec4f.h"
#include "FoxTools/Headers/App.h"

#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Errors/ErrorChecker.h"

#include "Usul/Errors/Assert.h"

#include "osg/Geometry"
#include "osg/Geode"
#include "osg/Texture2D"
#include "osg/TexEnv"
#include "osg/Matrix"
#include "osg/MatrixTransform"

using namespace Helios::Views;

///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( ImageView ) ImageViewMap[] =
{
  // Message type, id, and handler.
  FXMAPFUNC ( FX::SEL_CHANGED,         ImageView::ID_BACKGROUND_CHANGED,       ImageView::onBackgroundChanged  ), 
  FXMAPFUNC ( FX::SEL_CONFIGURE,       0,                                      ImageView::onResize             ),  
  FXMAPFUNC ( FX::SEL_PAINT,           0,                                      ImageView::onPaint             ),
};

// This class, base class, array of message handlers, size of the array.
FOX_TOOLS_IMPLEMENT ( ImageView, ImageView::BaseClass, ImageViewMap, ARRAYNUMBER ( ImageViewMap ) );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.  Do not use
//
///////////////////////////////////////////////////////////////////////////////

ImageView::ImageView() : BaseClass ()
{
  // Do not use
  USUL_ASSERT( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor 
//
///////////////////////////////////////////////////////////////////////////////

ImageView::ImageView ( Usul::Interfaces::IDocument* document, FX::FXComposite *parent, FX::FXGLVisual *visual ) : 
BaseClass ( parent, visual ),
_document ( document ),
_image(),
_viewer( new osgUtil::SceneView )
{
  _document->addView( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ImageView::~ImageView()
{
  _document->removeView ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create
//
///////////////////////////////////////////////////////////////////////////////

void ImageView::create()
{
  BaseClass::create();

  this->setFocus();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ImageView::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IBackground::IID:
    return static_cast< Usul::Interfaces::IBackground* > ( this );
  case Usul::Interfaces::IImageView::IID:
    return static_cast< Usul::Interfaces::IImageView* > ( this );
  case Usul::Interfaces::IViewer::IID:
    return static_cast< Usul::Interfaces::IViewer* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ref
//
///////////////////////////////////////////////////////////////////////////////

void ImageView::ref()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unref
//
///////////////////////////////////////////////////////////////////////////////

void ImageView::unref ( bool )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the background color.
//
///////////////////////////////////////////////////////////////////////////////

void ImageView::editBackground()
{
#if 0
  FX::FXColorDialog dialog ( FoxTools::Functions::mainWindow(), "Color Dialog", DECOR_TITLE | DECOR_BORDER );
  dialog.setOpaqueOnly ( dialog.isOpaqueOnly() );
  FX::FXVec4f c1 ( this->getBackColor( ) );
  dialog.setRGBA ( FX::FXVec4f ( c1[0], c1[1], c1[2], c1[3] ) );
  dialog.setTarget ( this );
  dialog.setSelector ( ImageView::ID_BACKGROUND_CHANGED );

  // Run the dialog in a modal loop.
  if ( dialog.execute ( FX::PLACEMENT_OWNER ) )
  {
    // Set the background color from the dialog's color.
    this->setBackColor ( dialog.getRGBA() );
  }

  // If the user cancelled...
  else
  {
    // Reset the color.
    this->setBackColor ( c1 );
  }

  // Whether the user set a new color or cancelled, we have to render again.
  // They may have cancelled after having moved the controls.
  this->render();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color to the default
//
///////////////////////////////////////////////////////////////////////////////

void ImageView::defaultBackground()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the old image and set new one
//
///////////////////////////////////////////////////////////////////////////////

void ImageView::setImage( osg::Image* img )
{
  _image = img;

  if( 0x0 == img )
  {
    _viewer->setSceneData( 0x0 );
  }
  else
  {
    // Get the dimensions.
	  const unsigned int width  ( _image->s() );
	  const unsigned int height ( _image->t() );

    // Make the geometry
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
    
    // Make the vertices
    osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array(4) );
    
    // Set the values
    vertices->at( 0 ).set( 0.0, height, 0.0 ); // Top left
    vertices->at( 1 ).set( 0.0, 0.0, 0.0 );    // Bottom left
    vertices->at( 2 ).set( width, 0.0, 0.0 );  // Bottom right
    vertices->at( 3 ).set( width, height, 0.0 ); // Top right
    
    // Make the texture coordinates
    osg::ref_ptr < osg::Vec2Array > texcoords ( new osg::Vec2Array(4) );

    // Set the values
    texcoords->at( 0 ).set( 0.0f, 1.0f );
    texcoords->at( 1 ).set( 0.0f, 0.0f );
    texcoords->at( 2 ).set( 1.0f, 0.0f );
    texcoords->at( 3 ).set( 1.0f, 1.0f );

    // Make the normals
    osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array(1) );
    
    // Set the value
    normals->at( 0 ).set( 0.0f, 0.0f, 1.0f );
    
    // Make the colors
    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array(1) );
    
    // Set the value
    colors->at ( 0 ).set( 1.0f, 1.0f, 1.0f, 1.0f );
    
    // Set the arrays
    geometry->setVertexArray   ( vertices.get() );
    geometry->setTexCoordArray ( 0, texcoords.get() );
    geometry->setNormalArray   ( normals.get() );
    geometry->setColorArray    ( colors.get() );

    // Set the bindings
    geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );
    geometry->setColorBinding  ( osg::Geometry::BIND_OVERALL );

    // Add the primitive set
    geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_QUADS, 0, 4 ) );
    
    // Make the geode
    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    
    // Add the geometry
    geode->addDrawable( geometry.get() );

    osg::ref_ptr < osg::Texture2D > texture ( new osg::Texture2D );
    texture->setDataVariance(osg::Object::DYNAMIC);
    texture->setImage( _image.get() );
    
    // Get or Create the state set
    osg::ref_ptr < osg::StateSet > stateset ( geometry->getOrCreateStateSet() );

    stateset->setMode( GL_LIGHTING, osg::StateAttribute::INHERIT );

    // Add the texture
    stateset->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON);

    stateset->setTextureAttributeAndModes(0,new osg::TexEnv(osg::TexEnv::MODULATE),osg::StateAttribute::ON);

#if 0
    osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
    mt->setMatrix ( osg::Matrix::translate ( ( this->getWidth() / 2 - width / 2 ), ( this->getHeight() / 2 - height / 2 ), 0 ) );
    mt->addChild( geode.get() );
    mt->setCullingActive( false );

    _viewer->setSceneData( mt.get() );
#else
    _viewer->setSceneData( geode.get() );
#endif

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the view
//
///////////////////////////////////////////////////////////////////////////////

void ImageView::render()
{
  this->makeCurrent();
  
  _viewer->setClearColor( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );

  _viewer->cull();

  _viewer->draw();
  this->swapBuffers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the view
//
///////////////////////////////////////////////////////////////////////////////

void ImageView::clearScene()
{
  _image = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get X
//
///////////////////////////////////////////////////////////////////////////////

int ImageView::x() 
{
  return this->getX();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Y
//
///////////////////////////////////////////////////////////////////////////////

int ImageView::y() 
{
  return this->getY();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the height
//
///////////////////////////////////////////////////////////////////////////////

int ImageView::height() 
{
  return this->getHeight();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the width
//
///////////////////////////////////////////////////////////////////////////////

int ImageView::width() 
{
  return this->getWidth();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle given message
//
///////////////////////////////////////////////////////////////////////////////

void ImageView::handleMessage ( unsigned short message )
{
  this->handle ( 0x0, FXSEL ( FX::SEL_COMMAND, message ), 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the background
//
///////////////////////////////////////////////////////////////////////////////

long ImageView::onBackgroundChanged ( FXObject* object, FX::FXSelector, void * )
{
#if 0
  FOX_TOOLS_ERROR_CHECKER ( TRUE == object->isMemberOf ( FXMETACLASS ( FX::FXColorDialog ) ) );
  FX::FXColorDialog *dialog = (FX::FXColorDialog *) object;
  this->setBackColor ( dialog->getRGBA() );
  image->render();
#endif

  return 1;
}


long ImageView::onResize( FX::FXObject *object, FX::FXSelector selector, void * callData )
{
  FX::FXEvent *event = (FX::FXEvent *) callData;

  // Resize the window.
  unsigned int width  ( static_cast < unsigned int > ( event->rect.w ) );
  unsigned int height ( static_cast < unsigned int > ( event->rect.h ) );
  
  if ( width >= 1 || height >= 1 )
  {
    if ( _viewer.valid() )
    {
      _viewer->setViewport ( 0, 0, (int) width, (int) height );
      _viewer->setProjectionMatrixAsOrtho ( 0, width, 0, height, -10, 10 );

#if 0
      osg::Node *root = _viewer->getSceneData();

      if( osg::MatrixTransform *mt = dynamic_cast< osg::MatrixTransform* > ( root ) )
      {
        const unsigned int width  ( _image->s() );
	      const unsigned int height ( _image->t() );

        mt->setMatrix ( osg::Matrix::translate ( ( this->getWidth() / 2 - width / 2 ), ( this->getHeight() / 2 - height / 2 ), 0 ) );
      }
#endif
    }
  }

  // Call the base class's function.
  return BaseClass::onConfigure ( object, selector, callData );
}



long ImageView::onPaint ( FX::FXObject *, FX::FXSelector, void * )
{
  this->render();

  return 1;
}

