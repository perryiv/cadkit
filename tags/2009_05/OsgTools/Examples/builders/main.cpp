///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "osg/Geode"
#include "osg/Group"
#include "osg/Vec3"
#include "osg/Matrix"
#include "osg/MatrixTransform"
#include "osgDB/ReadFile"

#include "osgProducer/Viewer"

#include "OsgTools/Circle.h"
#include "OsgTools/Box.h"
#include "OsgTools/Torus.h"
#include "OsgTools/Sphere.h"
#include "OsgTools/Axes.h"

osg::Group* create_scene();

int main(int argc, char* argv[])
{
  osg::ref_ptr<osg::Group> scene = create_scene();
  osg::ref_ptr<osgProducer::Viewer> viewer = new osgProducer::Viewer();
  viewer->setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);

  viewer->setSceneData( scene.get() );
  viewer->realize();

  while( !viewer->done() )
  {
    viewer->sync();
    viewer->update();
    viewer->frame();
  }

  viewer->sync();

  return 1;
}

osg::Group* create_scene()
{
  unsigned int types(7);

  osg::Vec4 BLUE  ( 0.0, 0.0, 1.0, 1.0 );
  osg::Vec4 GREEN ( 0.0, 1.0, 0.0, 1.0 );
  osg::Vec4 RED   ( 1.0, 0.0, 0.0, 1.0 );
  osg::Vec4 YELLOW( 1.0, 1.0, 0.0, 1.0 );

  // blue box
  osg::ref_ptr<osg::MatrixTransform> mtcbox = new osg::MatrixTransform();
  OsgTools::ColorBox cbox;
  cbox.color_policy().color( BLUE );
  mtcbox->addChild( cbox() );

  // red sphere
  osg::ref_ptr<osg::MatrixTransform> mtcsph = new osg::MatrixTransform();
  OsgTools::ColorSphere csphere;
  csphere.color_policy().color( RED );
  mtcsph->addChild( csphere() );

  // green torus
  osg::ref_ptr<osg::MatrixTransform> mttor = new osg::MatrixTransform();
  OsgTools::Torus ctorus( 0.7,                            // inner radius
                          1.0,                            // outer radius
                          osg::DegreesToRadians(   0.0 ), // start angle
                          osg::DegreesToRadians( 300.0 ), // end angle
                          50,                             // circle cuts
                          50 );                           // sweep cuts
  ctorus.setColor( GREEN );
  mttor->addChild( ctorus() );

  // origin
  osg::ref_ptr<osg::MatrixTransform> mtorigin = new osg::MatrixTransform();
  OsgTools::Axes origin;
  origin.state( OsgTools::Axes::POSITIVE_X |
                OsgTools::Axes::POSITIVE_Y |
                OsgTools::Axes::POSITIVE_Z |
                OsgTools::Axes::ORIGIN_CUBE );
  origin.colorX   ( YELLOW );
  origin.colorY   ( YELLOW );
  origin.colorZ   ( YELLOW );
  origin.colorBox ( YELLOW );
  mtorigin->addChild( origin() );

  // pole cursor
  osg::ref_ptr<osg::MatrixTransform> mtpole = new osg::MatrixTransform();
  OsgTools::Axes pole;
  pole.state( OsgTools::Axes::POSITIVE_Y |
              OsgTools::Axes::NEGATIVE_Y |
              OsgTools::Axes::ROTATE_Y     );
  mtpole->addChild( pole() );

  // fly/walk cursor
  osg::ref_ptr<osg::MatrixTransform> mtwalk = new osg::MatrixTransform();
  OsgTools::Axes walk;
  walk.state( OsgTools::Axes::POSITIVE_X |
              OsgTools::Axes::NEGATIVE_X |
              OsgTools::Axes::POSITIVE_Z |
              OsgTools::Axes::NEGATIVE_Z   );
  mtwalk->addChild( walk() );

  // load <cursor>.osg
  osg::ref_ptr<osg::MatrixTransform> mtext_cursor = new osg::MatrixTransform();
  //std::string cursor_name( "$OSGFILEPATH/cow.osg" );
  std::string cursor_name( "/home/users/hansenp/models/Cursors/zoom_in.flt" );
  osg::Node* ext_cursor = osgDB::readNodeFile( cursor_name.c_str() );
  mtext_cursor->addChild( ext_cursor );

  // mystery box
  osg::ref_ptr<osg::MatrixTransform> mtmbox = new osg::MatrixTransform();
  OsgTools::MaterialBox mbox;

  // front side material properties
  mbox.color_policy().ambient( osg::Vec4(0.6,0.6,0.6,1.0) );  // some light
  mbox.color_policy().diffuse( osg::Vec4(0.0,0.0,0.7,1.0) );  // blue color
  mbox.color_policy().emissive( osg::Vec4(0.0,0.0,0.0,1.0) ); // none
  mbox.color_policy().shininess( 0.0 );                       // no shine
  mbox.color_policy().specular( osg::Vec4(0.0,0.0,0.0,1.0) ); // no specular
  mtmbox->addChild( mbox() );

  // mystery sphere
  osg::ref_ptr<osg::MatrixTransform> mtmsph = new osg::MatrixTransform();
  OsgTools::MaterialSphere msphere;

  // front side material properties
  msphere.color_policy().ambient( osg::Vec4(0.8,0.8,0.8,1.0) );  // good light
  msphere.color_policy().diffuse( osg::Vec4(0.0,1.0,0.0,1.0) );  // bright green
  msphere.color_policy().emissive( osg::Vec4(0.0,0.0,0.0,1.0) ); // black
  msphere.color_policy().shininess( 0.5 );                       // some shine
  msphere.color_policy().specular( osg::Vec4(0.5,0.5,0.5,1.0) ); // some specular
  mtmsph->addChild( msphere() );

  OsgTools::Circle circle((float)types,types);
  osg::Vec3Array* cpoint = circle();

  mtcbox->setMatrix( osg::Matrix::translate((*cpoint)[0][0],
                                            (*cpoint)[0][1],
                                            (*cpoint)[0][2]) );

  //mtmbox->setMatrix( osg::Matrix::translate((*cpoint)[1][0],
  //                                          (*cpoint)[1][1],
  //                                          (*cpoint)[1][2]) );

  mtcsph->setMatrix( osg::Matrix::translate((*cpoint)[1][0],
                                            (*cpoint)[1][1],
                                            (*cpoint)[1][2]) );

  //mtmsph->setMatrix( osg::Matrix::translate((*cpoint)[3][0],
  //                                          (*cpoint)[3][1],
  //                                          (*cpoint)[3][2]) );

  mttor->setMatrix ( osg::Matrix::translate((*cpoint)[2][0],
                                            (*cpoint)[2][1],
                                            (*cpoint)[2][2]) );

  mtorigin->setMatrix ( osg::Matrix::translate((*cpoint)[3][0],
                                               (*cpoint)[3][1],
                                               (*cpoint)[3][2]) );

  mtpole->setMatrix ( osg::Matrix::translate((*cpoint)[4][0],
                                             (*cpoint)[4][1],
                                             (*cpoint)[4][2]) );

  mtwalk->setMatrix ( osg::Matrix::translate((*cpoint)[5][0],
                                             (*cpoint)[5][1],
                                             (*cpoint)[5][2]) );

  mtext_cursor->setMatrix ( osg::Matrix::translate((*cpoint)[6][0],
                                                   (*cpoint)[6][1],
                                                   (*cpoint)[6][2]) );

  osg::ref_ptr<osg::Group> group = new osg::Group;
  group->addChild( mtcbox.get() );
  //group->addChild( mtmbox.get() );
  group->addChild( mtcsph.get() );
  //->addChild( mtmsph.get() );
  group->addChild( mttor.get()  );
  group->addChild( mtorigin.get() );
  group->addChild( mtpole.get() );
  group->addChild( mtwalk.get() );
  group->addChild( mtext_cursor.get() );
  return group.release();
}
