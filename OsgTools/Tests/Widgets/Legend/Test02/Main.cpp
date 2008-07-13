
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "osgViewer/Viewer"

#include "OsgTools/Widgets/Legend.h"
#include "OsgTools/Widgets/LegendObject.h"
#include "OsgTools/Widgets/Text.h"
#include "OsgTools/Widgets/Image.h"

#include "Usul/Adaptors/Random.h"
#include "Usul/Strings/Format.h"

#include "osg/Camera"

#include <iostream>

int main ( int argc, char** argv )
{
  if ( argc < 2 )
  {
    std::cout << "Usage:  " << argv[0] << " image_filename" << std::endl;
    return -1;
  }
  
  osgViewer::Viewer viewer;
  viewer.getCamera()->setClearColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
  
  OsgTools::Widgets::Legend::RefPtr legend ( new OsgTools::Widgets::Legend );
  legend->maximiumSize ( 1024, 1024 );
  legend->heightPerItem ( 256 );
  legend->position ( 10, 10 );
  legend->growDirection ( OsgTools::Widgets::Legend::UP );
  
  OsgTools::Widgets::LegendObject::RefPtr row ( new OsgTools::Widgets::LegendObject );
    
  // Make an image.
  OsgTools::Widgets::Image::RefPtr icon ( new OsgTools::Widgets::Image ( argv[1] ) );
  icon->size ( Usul::Math::Vec2ui ( 256, 192 ) );
    
  // Make some long text  (from The Crisis, by Thomas Paine ).
  OsgTools::Widgets::Text::RefPtr text ( new OsgTools::Widgets::Text );
  text->text ( "THESE are the times that try men's souls. The summer soldier and the sunshine patriot will, in this crisis, shrink from the service of their country; but he that stands by it now, deserves the love and thanks of man and woman" );
  text->wrapLine ( true );
  text->alignmentVertical ( OsgTools::Widgets::Text::TOP );
  text->fontSize ( 45 );
  
  // Add the items.
  row->addItem ( icon.get() );
  row->addItem ( text.get() );

  // Set the percentage of the row.
  row->percentage ( 0 ) = 0.20;
  row->percentage ( 1 ) = 0.80;
    
  legend->addRow ( row.get() );
  
  osg::ref_ptr<osg::Camera> camera ( new osg::Camera );
  camera->setRenderOrder ( osg::Camera::POST_RENDER );
  camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  camera->setViewMatrix( osg::Matrix::identity() );
  camera->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
  camera->setCullingMode ( osg::CullSettings::NO_CULLING );
  camera->setViewport ( 0, 0, 1200, 1200 );
  camera->setProjectionMatrixAsOrtho2D ( 0, 1200, 0, 1200 );
  camera->addChild ( legend->buildScene() );
  
  viewer.setSceneData ( camera.get() );
  
  return viewer.run();
}
