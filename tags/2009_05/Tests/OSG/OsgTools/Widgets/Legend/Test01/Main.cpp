
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for Legend layout.
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


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef OsgTools::Widgets::Legend Legend;
typedef OsgTools::Widgets::LegendObject LegendRow;
typedef osg::Vec4 Color;


///////////////////////////////////////////////////////////////////////////////
//
//  Forward declare helper functions.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  LegendRow* makeRow ( const std::string& text );
  LegendRow* makeRow ( const Color& color, const std::string& text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char** argv )
{
  osgViewer::Viewer viewer;
  viewer.getCamera()->setClearColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );

  Legend::RefPtr outerLegend ( new Legend );
  outerLegend->maximiumSize ( 350, 600 );
  outerLegend->size ( 350, 600 );
  outerLegend->position ( 10, 10 ); 
  outerLegend->growDirection ( Legend::GROW_DIRECTION_UP );
  
  Legend::RefPtr legend ( new Legend );
  legend->maximiumSize ( 300, 600 );
  legend->position ( 5, 5 ); 
  legend->growDirection ( Legend::GROW_DIRECTION_UP );
  legend->borderColor ( Color ( 1.0, 0.0, 0.0, 1.0 ) );

  // Show only the border for the inner legend.
  legend->styleFlags ( Legend::STYLE_BORDER );

  // Make a row to hold the nested legend.
  LegendRow::RefPtr lo ( new LegendRow );
  lo->addItem ( legend.get() );
  lo->percentage ( 0 ) = 1.0;
  outerLegend->addRow ( lo.get() );

  // Add a row for a "title"
  outerLegend->addRow ( Helper::makeRow ( "Nested Legend Example" ) );

  const unsigned int items ( 5 );
  Usul::Adaptors::Random<float> random ( 0.0, 1.0 );
  
  // Generate rows.
  for ( unsigned int i = 0; i < items; ++ i )
  {
    Color color ( random(), random(), random(), 1.0 );
    std::string text ( Usul::Strings::format ( "Item ", i + 1 ) );

    legend->addRow ( Helper::makeRow ( color, text ) );
  }
  
  // Set up a camera.
  osg::ref_ptr<osg::Camera> camera ( new osg::Camera );
  camera->setRenderOrder ( osg::Camera::POST_RENDER );
  camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  camera->setViewMatrix( osg::Matrix::identity() );
  camera->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
  camera->setCullingMode ( osg::CullSettings::NO_CULLING );
  camera->setViewport ( 0, 0, 1024, 1024 );
  camera->setProjectionMatrixAsOrtho ( 0, 1024, 0, 1024, -100, 100 );
  camera->addChild ( outerLegend->buildScene() );
  
  viewer.setSceneData ( camera.get() );
  
  return viewer.run();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Make a row with text.
//
///////////////////////////////////////////////////////////////////////////////

LegendRow* Helper::makeRow ( const std::string& text )
{
  LegendRow::RefPtr row ( new LegendRow );

  // Make some text.
  OsgTools::Widgets::Text::RefPtr label ( new OsgTools::Widgets::Text );
  label->text ( text );  
  
  // Add the items.
  row->addItem ( label.get() );
  
  // Set the percentage of the row.
  row->percentage ( 0 ) = 1.0;

  return row.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a row with a color icon and text.
//
///////////////////////////////////////////////////////////////////////////////

LegendRow* Helper::makeRow ( const Color& color, const std::string& text )
{
  LegendRow::RefPtr row ( new LegendRow );
    
  // Make an icon.
  OsgTools::Widgets::SolidColorQuadIcon::RefPtr icon ( new OsgTools::Widgets::SolidColorQuadIcon );
  icon->colorPolicy().color ( color );
  icon->size ( Usul::Math::Vec2ui ( 20, 20 ) );
  
  // Make some text.
  OsgTools::Widgets::Text::RefPtr label ( new OsgTools::Widgets::Text );
  label->text ( text );  
  
  // Add the items.
  row->addItem ( icon.get() );
  row->addItem ( label.get() );
  
  // Set the percentage of the row.
  row->percentage ( 0 ) = 0.20;
  row->percentage ( 1 ) = 0.80;

  return row.release();
}
