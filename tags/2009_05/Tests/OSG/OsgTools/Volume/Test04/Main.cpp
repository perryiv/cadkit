
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
//  This is a test of gpu based ray casting.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functions/Color.h"
#include "Usul/Threads/Mutex.h"

#include "OsgTools/Box.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Volume/GPURayCasting.h"
#include "OsgTools/Volume/TransferFunction1D.h"

#include "osg/Projection"
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
  image->setImage( sizeX, sizeY, sizeZ, GL_INTENSITY, GL_LUMINANCE, GL_UNSIGNED_BYTE, volume, osg::Image::NO_DELETE );

  return image.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the transferFunction.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Volume::TransferFunction* buildTransferFunction ( )
{
  OsgTools::Volume::TransferFunction1D::RefPtr tf ( new OsgTools::Volume::TransferFunction1D );
  tf->size ( 256 );
  tf->textureUnit ( 1 );

  typedef OsgTools::Volume::TransferFunction1D::Color Color;

  for ( unsigned int i = 0; i < 256; ++i )
  {
    Color c;
    c[0] = i;
    c[1] = i;
    c[2] = i;
    c[3] = i < 10 ? 0 : 5;

    tf->value ( i, c );
  }

  /*Color red   ( 255, 0, 0, 25 );
  Color green ( 0, 255, 0, 25 );

  tf->value ( 127, red );
  tf->value ( 128, red );
  tf->value ( 129, red );
  tf->value ( 130, red );

  tf->value ( 200, green );
  tf->value ( 201, green );
  tf->value ( 202, green );
  tf->value ( 203, green );*/

  tf->value ( 245, Color ( 255, 127, 127, 55 ) );
  tf->value ( 246, Color ( 255, 127, 127, 55 ) );
  tf->value ( 247, Color ( 255, 127, 127, 55 ) );
  tf->value ( 248, Color ( 255, 127, 127, 55 ) );
  tf->value ( 249, Color ( 255, 127, 127, 55 ) );
  tf->value ( 250, Color ( 255, 127, 127, 55 ) );
  tf->value ( 251, Color ( 255, 127, 127, 55 ) );
  tf->value ( 252, Color ( 255, 127, 127, 55 ) );
  tf->value ( 253, Color ( 255, 127, 127, 55 ) );
  tf->value ( 254, Color ( 255, 127, 127, 55 ) );
  tf->value ( 255, Color ( 255, 127, 127, 55 ) );

  return tf.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the volume.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* buildVolume( const osg::BoundingBox& bb, const std::string& file, unsigned int xSize, unsigned int ySize, unsigned int zSize )
{
  osg::ref_ptr < OsgTools::Volume::GPURayCasting > volume ( new OsgTools::Volume::GPURayCasting );
  volume->samplingRate ( 0.01f );
  volume->image ( loadTexture ( xSize, ySize, zSize, file ) );
  volume->boundingBox ( bb );
  volume->transferFunction ( buildTransferFunction() );

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
  if ( argc < 5 )
    return -1;

  std::string filename ( argv[1] );

  unsigned int xSize ( 0 ), ySize ( 0 ), zSize ( 0 );

  {
    std::istringstream in ( argv [2] );
    in >> xSize;
  }

  {
    std::istringstream in ( argv [3] );
    in >> ySize;
  }

  {
    std::istringstream in ( argv [4] );
    in >> zSize;
  }

  osg::setNotifyLevel ( osg::DEBUG_FP );

  Usul::Threads::Mutex::createFunction ( Usul::Threads::newSingleThreadedMutexStub );

  osgViewer::Viewer viewer;
  viewer.getCamera()->setClearColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 0.0 ) );

  viewer.setUpViewInWindow( 0, 500, 512, 512 );

  osg::ref_ptr < osg::Group > root ( new osg::Group );

  //osg::BoundingBox bb ( osg::Vec3 ( -1.0, -1.0, -1.0 ), osg::Vec3 ( 1.0, 1.0, 1.0 ) );
  osg::BoundingBox bb ( osg::Vec3 ( 0.0, 0.0, 0.0 ), osg::Vec3 ( 1.0, 1.0, 1.0 ) );
  //osg::BoundingBox bb ( osg::Vec3 ( -0.5, -0.5, -0.5 ), osg::Vec3 ( 0.5, 0.5, 0.5 ) );

  //root->addChild ( buildBoundingBox( bb ) );

  //viewer.getCamera()->setProjectionMatrixAsOrtho ( 0.0, 512.0 , 0.0, 512.0, -10, 10 );
  //viewer.getCamera()->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
#if 0
  osg::ref_ptr < osg::Projection > projection ( new osg::Projection ( osg::Matrix::ortho ( 0.0, 512.0, 0.0, 512.0, -10, 10 ) ) );
  projection->addChild ( buildVolume( bb, filename, xSize, ySize, zSize ) );

  root->addChild ( projection.get() );
#else
  root->addChild ( buildVolume( bb, filename, xSize, ySize, zSize ) );
#endif

  viewer.setSceneData ( root.get() );

  viewer.realize();

	return viewer.run();
}
