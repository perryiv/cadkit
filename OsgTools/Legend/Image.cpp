
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Legend/Image.h"

#include "OsgTools/Font.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Geode"
#include "osg/Geometry"

#include "osgDB/ReadFile"

#include <iostream>

using namespace OsgTools::Legend;


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
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void Image::filename( const std::string& s )
{
  _filename = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text.
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
  const SizeType width  ( this->size()[0] );
  const SizeType height ( this->size()[1] );
  
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  
  geode->addDrawable ( 
        osg::createTexturedQuadGeometry (
            osg::Vec3 ( 0.0, 0.0, 0.0 ), osg::Vec3 ( width, 0.0, 0.0 ), osg::Vec3 ( 0.0, height, 0.0 ) ) );
  
  osg::ref_ptr<osg::Image> image ( osgDB::readImageFile ( this->filename() ) );
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
