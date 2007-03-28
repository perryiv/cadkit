  
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#include "Movie.h"

#include "osgDB/ReadFile"

#include "osg/Image"
#include "osg/ImageStream"
#include "osg/TextureRectangle"
#include "osg/Texture2D"

#include <iostream>

osg::ref_ptr< Cinema::Core::Movie > Cinema::Core::Movie::mInstance( 0x00 );

extern "C"  void loadPlugins();

///////////////////////////////////////////////////////////////////////////////
//
// Singleton. 
//
///////////////////////////////////////////////////////////////////////////////
Cinema::Core::Movie* Cinema::Core::Movie::instance()
{
  if( 0x00 == mInstance )
  {
    loadPlugins();
    mInstance = new Movie();
    return mInstance.get();
  }
  else
  {
    return mInstance.get();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Get scene root. 
//
///////////////////////////////////////////////////////////////////////////////   	   
osg::Node* Cinema::Core::Movie::root()
{
  return mRoot.get();
}

///////////////////////////////////////////////////////////////////////////////
//
// Get current state of the scene. 
//
///////////////////////////////////////////////////////////////////////////////
bool Cinema::Core::Movie::dirty() const 
{
  return mDirty;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set current state of the scene. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::dirty( bool val )
{
  mDirty = val;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get whether the position is used as center or corner point for a quad. 
//
///////////////////////////////////////////////////////////////////////////////
bool Cinema::Core::Movie::usePositionAsCenter() const
{
  return mUsePositionAsCenterPoint;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set whether the position  shoule be used as center or corner point for a quad. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::usePositionAsCenter( bool val )
{   
  mUsePositionAsCenterPoint = val; 
  this->dirty( true );
}

///////////////////////////////////////////////////////////////////////////////
//
// Get current screen shape. 
//
///////////////////////////////////////////////////////////////////////////////
Cinema::Core::Movie::ScreenShape Cinema::Core::Movie::screenShape() const 
{
  return mScreenShape;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set current screen shape.
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::screenShape( Cinema::Core::Movie::ScreenShape shape )
{
  mScreenShape = shape;
  this->dirty( true );
}

///////////////////////////////////////////////////////////////////////////////
//
// Get current screen width. 
//
///////////////////////////////////////////////////////////////////////////////
const float& Cinema::Core::Movie::screenWidth() const
{
  return mScreenWidth;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set current screen width. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::screenWidth( const float& width )
{
  mScreenWidth = width;
  this->dirty( true );
}

///////////////////////////////////////////////////////////////////////////////
//
// Get current screen height. 
//
///////////////////////////////////////////////////////////////////////////////
const float& Cinema::Core::Movie::screenHeight() const
{
  return mScreenHeight;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set current screen width. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::screenHeight( const float& height )
{
  mScreenHeight = height;
  this->dirty( true );
}

const osg::Vec3f& Cinema::Core::Movie::screenWidthVector() const 
{
  return mScreenWidthVector;
}

void Cinema::Core::Movie::screenWidthVector( const osg::Vec3f& vec )
{
  mScreenWidthVector = vec;
  this->dirty( true );
}

const osg::Vec3f& Cinema::Core::Movie::screenHeightVector() const 
{
  return mScreenHeightVector;
}

void Cinema::Core::Movie::screenHeightVector( const osg::Vec3f& vec )
{
  mScreenHeightVector = vec;
  this->dirty( true );
}

///////////////////////////////////////////////////////////////////////////////
//
// Get current screen position( corner or center point ). 
//
///////////////////////////////////////////////////////////////////////////////
const osg::Vec3f& Cinema::Core::Movie::screenPosition() const
{
  return mPosition;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set current screen position( corner or center point ). 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::screenPosition( osg::Vec3f  position )
{
  mPosition = position;
  this->dirty( true );
}

///////////////////////////////////////////////////////////////////////////////
//
// Set current movie filename. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::fileName( const std::string& file )
{
  mFileName = file;
  this->dirty( true );
}

///////////////////////////////////////////////////////////////////////////////
//
// Set movie filename to play and the position for its canvas.
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::setMovie( const osg::Vec3f& position, const std::string& file )
{
  // The way this function would work is we would drop all the current movies playing 
  // and we will rebuild everything. 
  mPosition = position;
  mFileName = file;

  this->dirty( true );
}

///////////////////////////////////////////////////////////////////////////////
//
// Set movie filename to play and two corners of the quad( canvas ). 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::setMovie( const osg::Vec3f& position, const osg::Vec3f& widthVector, const osg::Vec3f& heightVector, const std::string& file )
{ 
  mPosition           = position;
  mScreenWidthVector  = widthVector;
  mScreenHeightVector = heightVector;

  mFileName           =  file;

  this->dirty( true );
}

///////////////////////////////////////////////////////////////////////////////
//
// Set movie filename to play, screen width, screen height and its position. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::setMovie( const float& width, const float& height, const osg::Vec3f& position, const std::string& file ) 
{
  mScreenWidth  = width;
  mScreenHeight = height;
  mPosition     = position;
  mFileName     = file;

  this->dirty( true );
}
///////////////////////////////////////////////////////////////////////////////
//
// Play movie. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::play()
{
  if( 0x00 != mMovieHandler.get() )
  {
    try
    {
      mMovieHandler->action( MovieHandler::PLAY );
      mMovieHandler->update();
    }
    catch( const std::exception& e )
    {
      std::cerr << "Error ErrorNo " << std::endl;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Pause movie. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::pause()
{
  if( 0x00 != mMovieHandler.get() )
  {
    mMovieHandler->action( MovieHandler::PAUSE );
    mMovieHandler->update();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Restart movie. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::restart()
{
  if( 0x00 != mMovieHandler.get() )
  {
    mMovieHandler->action( MovieHandler::RESTART );
    mMovieHandler->update();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Set loop true/false. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::loop( bool val )
{
  if( 0x00 != mMovieHandler.get() )
  {
    if( val ){ mMovieHandler->mode( MovieHandler::LOOP ); }
    else     { mMovieHandler->mode( MovieHandler::NO_LOOP ); }
    mMovieHandler->update();
  }
}

bool Cinema::Core::Movie::isPlaying()
{
  return mMovieHandler->isPlaying();
}

bool Cinema::Core::Movie::isPaused()
{
  return mMovieHandler->isPaused();
}

///////////////////////////////////////////////////////////////////////////////
//
// Build geometry, set streams for a movie to play. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::Movie::buildScene()
{ 	  
  try
  {
    if( this->dirty() )
    {
      mBillboard->removeDrawables( 0, mBillboard->getNumDrawables() );   
      mMovieHandler->cleanUp();

      osg::ref_ptr< osg::Image > image = osgDB::readImageFile( mFileName );
      if( image.valid() )
      {        
        osg::ref_ptr< osg::ImageStream > imageStream = dynamic_cast< osg::ImageStream* >( image.get() );

        // Checking for NULL pointer is not enough as we can have a valid image pointer but invalid stream. 
        //if( imageStream.valid() && ( imageStream->getStatus() != osg::ImageStream::INVALID )  )

        if( imageStream.valid() )
        {      	  
          imageStream->play();
        }

        if( mBuildGeometry )
        { 
      	  std::cout << "Build geometry " << std::endl;
          if( mScreenWidth == 0.0 )
          {
            mScreenWidth = image->s();            
          }
          if( mScreenHeight == 0.0 )
          {
            mScreenHeight = image->t();            
          }	       
  	
          mBillboard->addDrawable( buildGeometry( image.get() ) );	      

          if( mAllowRotation )
          {
            mBillboard->setMode( osg::Billboard::POINT_ROT_EYE );       
          }      

          mMovieHandler->set( mRoot.get() );
        }
        else
        {
          // Else find the geometry from the a given node / geode. 
        }

        this->dirty( false );
      }
    }
  }
  catch(...)
  {
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Build geometry for the screen( canvas ). 
//
///////////////////////////////////////////////////////////////////////////////
osg::Geometry* Cinema::Core::Movie::buildGeometry( osg::Image* image )
{
  switch( mScreenShape )
  {
    case Cinema::Core::Movie::QUAD:
    {
      return buildQuad( image );
      break;
    }
    default:
    {
      throw std::string( "ScreenShape not handled: " );
    }
  };
}

///////////////////////////////////////////////////////////////////////////////
//
// Build quad geometry for the screen( canvas ). 
//
///////////////////////////////////////////////////////////////////////////////
osg::Geometry* Cinema::Core::Movie::buildQuad( osg::Image* image )
{
  if( mUseTextureRectangle )
  {
    if( mScreenWidthVector.length() == 0.0 )
    {
      mScreenWidthVector.set( mScreenWidth, 0.0, 0.0 );
    }

    if( mScreenHeightVector.length() == 0.0 )
    {
      mScreenHeightVector.set( 0.0, 0.0, mScreenHeight );
    }

    osg::ref_ptr< osg::Geometry >  pictureQuad = osg::createTexturedQuadGeometry( 
                                                   mPosition,
                                                   mScreenWidthVector,
                                                   mScreenHeightVector,
                                                   0.0f, image->t(), image->s(), 0.0f );

    pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes( 0, new osg::TextureRectangle( image ), osg::StateAttribute::ON );
                  
    return pictureQuad.release();
   }
  else
  {
    osg::ref_ptr< osg::Geometry > pictureQuad = osg::createTexturedQuadGeometry( 
                                                  mPosition,
                                                  mScreenWidthVector,
                                                  mScreenHeightVector,
                                                  0.0f, 1.0f, 1.0f, 0.0f );
                                  
    osg::ref_ptr< osg::Texture2D > texture    = new osg::Texture2D( image );
    texture->setFilter( osg::Texture::MIN_FILTER,osg::Texture::LINEAR );  
                                     
    pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON );

    return pictureQuad.release();
  }
}
