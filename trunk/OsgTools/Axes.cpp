///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV and John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Coordinate axes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Axes.h"
#include "ErrorChecker.h"
#include "Torus.h"
#include "Box.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/ShapeDrawable"
#include "osg/MatrixTransform"

#include "osgUtil/Optimizer"

#include "Usul/Bits/Bits.h"

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Axes::Axes() :
  _colorX ( 1, 0, 0, 1 ),
  _colorY ( 0, 1, 0, 1 ),
  _colorZ ( 0, 0, 1, 1 ),
  _colorBox ( 1, 1, 1, 1 ),
  _length ( 0.5f ),
  _width ( 0.05f ),
  _state( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build coordinate axes.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Axes::operator()() const
{
  osg::ref_ptr<osg::Group> group = new osg::Group;  // place it all under a group

  if( Usul::Bits::has(_state,POSITIVE_X) )
  {
    osg::Vec3 about(0.0,1.0,0.0);  // y-axis
    double amount(osg::inDegrees(90.0));
    osg::ref_ptr<osg::Group> axis = cartesian_graphic(_colorX,osg::Quat(amount,about));
    group->addChild( axis.get() );
    //std::cout << "Axes: added: X" << std::endl;
  }

  if( Usul::Bits::has(_state,POSITIVE_Y) )
  {
    osg::Vec3 about(1.0,0.0,0.0);   // x-axis
    double amount(osg::inDegrees(270.0));
    osg::ref_ptr<osg::Group> axis = cartesian_graphic(_colorY,osg::Quat(amount,about));
    group->addChild( axis.get() );
    //std::cout << "Axes: added: Y" << std::endl;
  }

  if( Usul::Bits::has(_state,POSITIVE_Z) )
  {
    osg::ref_ptr<osg::Group> axis = cartesian_graphic(_colorZ);
    group->addChild( axis.get() );
    //std::cout << "Axes: added: Z" << std::endl;
  }

  if( Usul::Bits::has(_state,NEGATIVE_X) )
  {
    osg::Vec3 about(0.0,1.0,0.0);  // y-axis
    double amount(osg::inDegrees(270.0));           // -90.0 degrees
    osg::ref_ptr<osg::Group> axis = cartesian_graphic(_colorX,osg::Quat(amount,about));
    group->addChild( axis.get() );
    //std::cout << "Axes: added: -X" << std::endl;
  }

  if( Usul::Bits::has(_state,NEGATIVE_Y) )
  {
    osg::Vec3 about(1.0,0.0,0.0);  // x-axis
    double amount(osg::inDegrees(90.0));
    osg::ref_ptr<osg::Group> axis = cartesian_graphic(_colorY,osg::Quat(amount,about));
    group->addChild( axis.get() );
    //std::cout << "Axes: added: -Y" << std::endl;
  }

  if( Usul::Bits::has(_state,NEGATIVE_Z) )
  {
    osg::Vec3 about(1.0,0.0,0.0);  // x-axis
    double amount(osg::inDegrees(180.0));
    osg::ref_ptr<osg::Group> axis = cartesian_graphic(_colorZ,osg::Quat(amount,about));
    group->addChild( axis.get() );
    //std::cout << "Axes: added: -Z" << std::endl;
  }

  if( Usul::Bits::has(_state,ROTATE_X) )
  {
    osg::Vec3 about(0.0,1.0,0.0);  // y-axis
    double amount(osg::inDegrees(90.0));         // 90.0 degrees
    osg::ref_ptr<osg::Group> torus = torus_graphic(_colorX,osg::Quat(amount,about));
    group->addChild( torus.get() );
    //std::cout << "Axes: added: rotate along X" << std::endl;
  }

  if( Usul::Bits::has(_state,ROTATE_Y) )
  {
    osg::Vec3 about(1.0,0.0,0.0);  // x-axis
    double amount(osg::inDegrees(270.0));        // -90.0 degrees
    osg::ref_ptr<osg::Group> torus = torus_graphic(_colorY,osg::Quat(amount,about));
    group->addChild( torus.get() );
    //std::cout << "Axes: added: rotate along Y" << std::endl;
  }

  if( Usul::Bits::has(_state,ROTATE_Z) )
  {
    osg::ref_ptr<osg::Group> torus = torus_graphic(_colorZ);
    group->addChild( torus.get() );
    //std::cout << "Axes: added: rotate along Z" << std::endl;
  }

  // Create a box at the origin
  if( Usul::Bits::has(_state,ORIGIN_CUBE) )
  {
    osg::ref_ptr<osg::Geode> box = box_graphic(_colorBox);
    group->addChild(box.get());
  }

  group->setDataVariance(osg::Object::STATIC);

  osgUtil::Optimizer mizer;
  mizer.optimize( group.get(), osgUtil::Optimizer::ALL_OPTIMIZATIONS );

  return( group.release() );
}

// returns a Geode that looks like an arrow
// by default, down the z-axis
osg::Group* Axes::cartesian_graphic(const osg::Vec4& color,const osg::Quat& quat) const
{
  ErrorChecker ( 1068242097, _length > 0 );
  ErrorChecker ( 1068242098, _width > 0 );

  osg::ref_ptr<osg::Geode> base ( new osg::Geode );
  osg::ref_ptr<osg::Geode> top ( new osg::Geode );

  osg::Vec3 center(0.0,0.0,0.5*_length);  // useful vector

  // create the cylinders for the axes
  osg::ref_ptr<osg::Cylinder> cylinder = new osg::Cylinder;
  cylinder->set(center, _width, _length);

  osg::ref_ptr<osg::ShapeDrawable> shaft = new osg::ShapeDrawable;
  shaft->setShape(cylinder.get());
  shaft->setColor( color );

  osg::ref_ptr<osg::Cone> cone = new osg::Cone;
  cone->set(osg::Vec3(0.0f,0.0f,0.0f), _width*1.4, _length*0.1);

  osg::ref_ptr<osg::ShapeDrawable> hat = new osg::ShapeDrawable;
  hat->setShape( cone.get() );
  hat->setColor( color );

  // add the drawables to each geode
  base->addDrawable( shaft.get() );
  top->addDrawable( hat.get() );

  // move the top out to the end of the base
  osg::Matrix top_trans;
  osg::Vec3 distance(center[0],center[1],center[2]+0.525*_length);
  top_trans.makeTranslate( distance );
  osg::ref_ptr<osg::MatrixTransform> top_xform = new osg::MatrixTransform;
  top_xform->setMatrix( top_trans );
  top_xform->addChild( top.get() );

  osg::ref_ptr<osg::Group> group = new osg::Group;

  // rotate geomtery if necessary
  if( !quat.zeroRotation() )
    {
      osg::Matrix rotate;
      rotate.makeRotate( quat );

      osg::ref_ptr<osg::MatrixTransform> base_xform = new osg::MatrixTransform;
      base_xform->setMatrix( rotate );
      base_xform->addChild( base.get() );
      base_xform->addChild( top_xform.get() );

      group->addChild( base_xform.get() );
    }

  else
    {
      group->addChild( top_xform.get() );
      group->addChild( base.get() );
    }

  // group->setDataVariance(osg::Object::STATIC);
  return( group.release() );
}

osg::Group* Axes::torus_graphic(const osg::Vec4& color, const osg::Quat& quat) const
{
  // make the torus
  float radi = 5.0*_width;
  Torus torus(radi,radi+_width*2.0,0.0,osg::DegreesToRadians(270.0));
  torus.setColor( color );
  osg::ref_ptr<osg::Geode> torus_gfx = torus();

  // make the cone
  osg::ref_ptr<osg::Cone> cone = new osg::Cone();
  cone->set(osg::Vec3(0.0f,0.0f,0.0f), _width*1.4, _length*0.1);
  osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(cone.get());
  sd->setColor( color );
  osg::ref_ptr<osg::Geode> cone_geode = new osg::Geode;
  cone_geode->addDrawable( sd.get() );

  // position the cone
  osg::Quat turn_it(osg::DegreesToRadians(90.0),osg::Vec3(0.0,1.0,0.0));
  osg::Vec3 move_it(0.0,-(radi+_width),0.0);
  osg::Matrix move;   move.makeTranslate( move_it );
  osg::ref_ptr<osg::MatrixTransform> coneXform = new osg::MatrixTransform;
  coneXform->addChild( cone_geode.get() );
  coneXform->setMatrix( osg::Matrix(turn_it) );
  coneXform->preMult( move );

  // group the graphics together
  osg::ref_ptr<osg::MatrixTransform> group = new osg::MatrixTransform;
  group->addChild( torus_gfx.get() );
  group->addChild( coneXform.get() );

  // move the torus+cone graphic
  //osg::Matrix translate;
  //translate.makeTranslate( osg::Vec3(0.0,0.0,0.5*_length) );
  // John, I do not think translation is needed, but I did not test every case... --Per

  // rotate the graphic if necessary
  if( quat.zeroRotation() )
    {
      //group->preMult( translate );
      return( group.release() );
    }

  else
    {
      //std::cout << "rotating torus" << std::endl;
      osg::Matrix rotate;
      rotate.makeRotate( quat );
      //group->preMult( translate*rotate );
      group->preMult( rotate );
    }

  return( group.release() );
}

osg::Geode* Axes::box_graphic(const osg::Vec4& color) const
{
  float size = _width*2.3;
  Box box(size,size,size);
  box.setColor( color );

  osg::ref_ptr<osg::Geode> geode = box();

  return( geode.release() );
}
