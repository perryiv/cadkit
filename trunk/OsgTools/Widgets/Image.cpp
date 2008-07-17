
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/Image.h"

#include "OsgTools/Font.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Geode"
#include "osg/Geometry"

#include "osgDB/ReadFile"

#include <iostream>

using namespace OsgTools::Widgets;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Image::Image() : 
  BaseClass(), 
  _filename()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Image::Image( const std::string& filename ) : 
  BaseClass(), 
  _filename( filename )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Image::~Image()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the filename.
//
///////////////////////////////////////////////////////////////////////////////

void Image::filename( const std::string& s )
{
  _filename = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filename.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Image::filename() const
{
  return _filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Image::buildScene()
{
  // Read the image
  osg::ref_ptr<osg::Image> image ( osgDB::readImageFile ( this->filename() ) );
  
  // Return an null node if we don't have an image.  TODO: Return some sort of proxy geometry instead.
  if ( false == image.valid() )
    return 0x0;
  
  const Size size ( Image::_fitImageToSize ( this->size(), *image ) );
  const SizeType width  ( size[0] );
  const SizeType height ( size[1] );
  
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  
  geode->addDrawable ( 
        osg::createTexturedQuadGeometry (
            osg::Vec3 ( 0.0, 0.0, 0.0 ), osg::Vec3 ( width, 0.0, 0.0 ), osg::Vec3 ( 0.0, height, 0.0 ) ) );
  
  
  osg::ref_ptr<osg::Texture2D> texture ( new osg::Texture2D );
  texture->setImage ( image.get() );
  
  osg::ref_ptr<osg::StateSet> ss ( geode->getOrCreateStateSet() );
  ss->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  //ss->setRenderBinDetails ( 11, "RenderBin" );
  ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
  
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( geode.get(), false );
  
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the size for the image to fit into given size.  The preserves aspect ratio.
//
///////////////////////////////////////////////////////////////////////////////

Image::Size Image::_fitImageToSize ( const Size& size, const osg::Image& image )
{
  Size newSize ( size );
  
  const int s ( image.s() );
  const int t ( image.t() );
  
  const double aspect ( static_cast<double> ( s ) / static_cast<double> ( t ) );
  
  const SizeType width ( size[0] );
  
  if ( s > static_cast<int> ( width ) )
  {
    newSize[0] = width;
    newSize[1] = width * ( aspect < 1.0 ? ( 1.0 / aspect ) : aspect );
  }
  
  return newSize;
}
