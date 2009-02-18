///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Sample.h"
#include "osg/Vec4"
#include "osg/MatrixTransform"
#include "OsgTools/Circle.h"
#include "OsgTools/Box.h"
#include "OsgTools/Torus.h"
#include "OsgTools/Sphere.h"
#include "OsgTools/Axes.h"

using namespace OsgTools;

osg::Node* Sample::make_scene()
{
  const unsigned int variables(6);

  osg::Vec4 BLUE  ( 0.0, 0.0, 1.0, 1.0 );
  osg::Vec4 GREEN ( 0.0, 1.0, 0.0, 1.0 );
  osg::Vec4 RED   ( 1.0, 0.0, 0.0, 1.0 );
  osg::Vec4 YELLOW( 1.0, 1.0, 0.0, 1.0 );

  // blue box
  osg::ref_ptr<osg::MatrixTransform> mtcbox = new osg::MatrixTransform();
  mtcbox->setName("OsgTools_Sample_scene_mtcbox");
  OsgTools::ColorBox cbox;
  cbox.color_policy().color( BLUE );
  mtcbox->addChild( cbox() );

  // red sphere
  osg::ref_ptr<osg::MatrixTransform> mtcsph = new osg::MatrixTransform();
  mtcbox->setName("OsgTools_Sample_scene_mtcsph");
  OsgTools::ColorSphere csphere;
  csphere.color_policy().color( RED );
  mtcsph->addChild( csphere() );

  // green torus
  osg::ref_ptr<osg::MatrixTransform> mttor = new osg::MatrixTransform();
  mtcbox->setName("OsgTools_Sample_scene_mttor");
  OsgTools::Torus ctorus( 0.7f,                            // inner radius
                          1.0f,                            // outer radius
                          osg::DegreesToRadians(   0.0 ), // start angle
                          osg::DegreesToRadians( 300.0 ), // end angle
                          50,                             // circle cuts
                          50 );                           // sweep cuts
  ctorus.setColor( GREEN );
  mttor->addChild( ctorus() );

  // origin
  osg::ref_ptr<osg::MatrixTransform> mtorigin = new osg::MatrixTransform();
  mtcbox->setName("OsgTools_Sample_scene_mtorigin");
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
  mtcbox->setName("OsgTools_Sample_scene_mtpole");
  OsgTools::Axes pole;
  pole.state( OsgTools::Axes::POSITIVE_Y |
              OsgTools::Axes::NEGATIVE_Y |
              OsgTools::Axes::ROTATE_Y     );
  mtpole->addChild( pole() );

  // fly/walk cursor
  osg::ref_ptr<osg::MatrixTransform> mtwalk = new osg::MatrixTransform();
  mtcbox->setName("OsgTools_Sample_scene_mtwalk");
  OsgTools::Axes walk;
  walk.state( OsgTools::Axes::POSITIVE_X |
              OsgTools::Axes::NEGATIVE_X |
              OsgTools::Axes::POSITIVE_Z |
              OsgTools::Axes::NEGATIVE_Z   );
  mtwalk->addChild( walk() );

  // mystery box
  osg::ref_ptr<osg::MatrixTransform> mtmbox = new osg::MatrixTransform();
  mtcbox->setName("OsgTools_Sample_scene_mtmbox");
  OsgTools::MaterialBox mbox;

  // front side material properties
  mbox.color_policy().ambient( osg::Vec4(0.6f,0.6f,0.6f,1.0f) );  // some light
  mbox.color_policy().diffuse( osg::Vec4(0.0f,0.0f,0.7f,1.0f) );  // blue color
  mbox.color_policy().emissive( osg::Vec4(0.0f,0.0f,0.0f,1.0f) ); // none
  mbox.color_policy().shininess( 0.0f );                       // no shine
  mbox.color_policy().specular( osg::Vec4(0.0f,0.0f,0.0f,1.0f) ); // no specular
  mtmbox->addChild( mbox() );

  // mystery sphere
  osg::ref_ptr<osg::MatrixTransform> mtmsph = new osg::MatrixTransform();
  mtcbox->setName("Sample_scene_mtmsph");
  OsgTools::MaterialSphere msphere;
  msphere.radius(1.2*msphere.radius());

  // front side material properties
  msphere.color_policy().ambient( osg::Vec4(0.8f,0.8f,0.8f,1.0f) );  // good light
  msphere.color_policy().diffuse( osg::Vec4(0.0f,1.0f,0.0f,1.0f) );  // bright green
  msphere.color_policy().emissive( osg::Vec4(0.0f,0.0f,0.0f,1.0f) ); // black
  msphere.color_policy().shininess( 0.5f );                       // some shine
  msphere.color_policy().specular( osg::Vec4(0.5f,0.5f,0.5f,1.0f) ); // some specular
  mtmsph->addChild( msphere() );

  OsgTools::Circle circle(3.0f,variables);
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

  osg::Group* group = new osg::Group();
  mtcbox->setName("OsgTools_Sample_scene_group");
  group->addChild( mtcbox.get() );
  group->addChild( mtmbox.get() );
  group->addChild( mtcsph.get() );
  group->addChild( mtmsph.get() );
  group->addChild( mttor.get()  );
  group->addChild( mtorigin.get() );
  group->addChild( mtpole.get() );
  group->addChild( mtwalk.get() );
  return group;
}
