#include "osg/Geode"
#include "osg/Group"
#include "osg/Vec3"
#include "osg/Matrix"
#include "osg/MatrixTransform"

#include "osgProducer/Viewer"

#include "OsgTools/Circle.h"
#include "OsgTools/Box.h"
#include "OsgTools/Torus.h"
#include "OsgTools/Sphere.h"

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

  return 0;
}

osg::Group* create_scene()
{
  unsigned int types(4);

  osg::Vec4 BLUE(0.0,0.0,1.0,1.0);
  osg::Vec4 GREEN(0.0,1.0,0.0,1.0);
  osg::Vec4 RED(1.0,0.0,0.0,1.0);

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

  mtmbox->setMatrix( osg::Matrix::translate((*cpoint)[1][0],
                                            (*cpoint)[1][1],
                                            (*cpoint)[1][2]) );

  mtcsph->setMatrix( osg::Matrix::translate((*cpoint)[2][0],
                                            (*cpoint)[2][1],
                                            (*cpoint)[2][2]) );

  mtmsph->setMatrix( osg::Matrix::translate((*cpoint)[3][0],
                                            (*cpoint)[3][1],
                                            (*cpoint)[3][2]) );

  osg::ref_ptr<osg::Group> group = new osg::Group;
  group->addChild( mtcbox.get() );
  group->addChild( mtmbox.get() );
  group->addChild( mtcsph.get() );
  group->addChild( mtmsph.get() );
  return group.release();
}
