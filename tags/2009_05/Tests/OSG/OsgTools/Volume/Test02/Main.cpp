
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This is a test of the volume node.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Box.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Volume/Texture3DVolume.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include "osgViewer/Viewer"


///////////////////////////////////////////////////////////////////////////////
//
//  Load the file.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* loadTexture( int sizeX, int sizeY, int sizeZ, const std::string& filename )
{ 
  FILE *pFile = fopen( filename.c_str(), "rb" );
  		
  if ( NULL == pFile ) 
  {
	  return 0x0;
  }

  unsigned int size ( sizeX * sizeY * sizeZ );

  unsigned char *volume = new unsigned char[ size ];

  fread( volume, sizeof ( unsigned char ), size, pFile );
  fclose( pFile );

  osg::ref_ptr < osg::Image > image ( new osg::Image );
  image->setImage( 256, 256, 256, GL_INTENSITY, GL_LUMINANCE, GL_UNSIGNED_BYTE, volume, osg::Image::NO_DELETE );

  return image.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the volume.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* buildVolume( const osg::BoundingBox& bb )
{
  osg::ref_ptr < OsgTools::Volume::Texture3DVolume > volume ( new OsgTools::Volume::Texture3DVolume );
  volume->numPlanes ( 512 );
  volume->image ( loadTexture ( 256, 256, 256, "Engine256.raw" ) );
  volume->boundingBox ( bb );

  return volume.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the bounding box to draw.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* buildBoundingBox( const osg::BoundingBox& bb )
{
  OsgTools::ColorBox box ( bb );
  box.color_policy().color ( osg::Vec4 ( 0, 0, 1, 1 ) );

  // Position it.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( bb.center() ) );
  mt->addChild ( box() );

  // Wire-frame.
  OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
  OsgTools::State::StateSet::setLighting ( mt.get(), false );
	
	return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main( int argc, char **argv )
{
  osgViewer::Viewer viewer;

  viewer.setUpViewInWindow( 0, 500, 512, 512 );

  osg::ref_ptr < osg::Group > root ( new osg::Group );

  osg::BoundingBox bb ( osg::Vec3 ( -1.0, -1.0, -1.0 ), osg::Vec3 ( 1.0, 1.0, 1.0 ) );

  root->addChild ( buildBoundingBox( bb ) );
  root->addChild ( buildVolume( bb ) );

  viewer.setSceneData ( root.get() );

  viewer.realize();

	return viewer.run();
}
