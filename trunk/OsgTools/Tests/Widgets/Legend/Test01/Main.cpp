
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
#include "OsgTools/Widgets/SolidIcon.h"

#include "Usul/Adaptors/Random.h"
#include "Usul/Strings/Format.h"

#include "osg/Camera"

int main ( int argc, char** argv )
{
  osgViewer::Viewer viewer;
  viewer.getCamera()->setClearColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
  
  OsgTools::Widgets::Legend::RefPtr legend ( new OsgTools::Widgets::Legend );
  legend->maximiumSize ( 300, 600 );
  //legend->heightPerItem ( 40 );
  legend->position ( 10, 10 );
  legend->growDirection ( OsgTools::Widgets::Legend::UP );
  
  const unsigned int items ( 5 );
  Usul::Adaptors::Random<float> random ( 0.0, 1.0 );
  
  for ( unsigned int i = 0; i < items; ++ i )
  {
    OsgTools::Widgets::LegendObject::RefPtr lo ( new OsgTools::Widgets::LegendObject );
    
    // Make an icon.
    OsgTools::Widgets::SolidColorQuadIcon::RefPtr icon ( new OsgTools::Widgets::SolidColorQuadIcon );
    icon->colorPolicy().color ( osg::Vec4 ( random(), random(), random(), 1.0 ) );
    icon->size ( Usul::Math::Vec2ui ( 20, 20 ) );
    
    // Make some text.
    OsgTools::Widgets::Text::RefPtr text ( new OsgTools::Widgets::Text );
    text->text ( Usul::Strings::format ( "Item ", i + 1 ) );
    
    // Add the items.
    lo->addItem ( icon.get() );
    lo->addItem ( text.get() );
    
    // Set the percentage of the row.
    lo->percentage ( 0 ) = 0.20;
    lo->percentage ( 1 ) = 0.80;
    
    legend->addRow ( lo.get() );
  }
  
  osg::ref_ptr<osg::Camera> camera ( new osg::Camera );
  camera->setRenderOrder ( osg::Camera::POST_RENDER );
  camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  camera->setViewMatrix( osg::Matrix::identity() );
  camera->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
  camera->setCullingMode ( osg::CullSettings::NO_CULLING );
  camera->setViewport ( 0, 0, 1024, 1024 );
  camera->setProjectionMatrixAsOrtho2D ( 0, 1024, 0, 1024 );
  camera->addChild ( legend->buildScene() );
  
  viewer.setSceneData ( camera.get() );
  
  return viewer.run();
}
