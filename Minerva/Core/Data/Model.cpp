
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/Model.h"

#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"

#include "OsgTools/State/StateSet.h"

#include "osg/MatrixTransform"

#include "osgUtil/Optimizer"

using namespace Minerva::Core::Data;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Model::Model() :
  BaseClass(),
  _location(),
  _heading ( 0.0 ),
  _tilt ( 0.0 ),
  _roll ( 0.0 ),
  _toMeters ( 0.0254 ),
  _scale ( 1.0, 1.0, 1.0 ),
  _model ( 0x0 ),
  _optimize ( true )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Model::~Model()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Model::_buildScene( Usul::Interfaces::IUnknown* caller )
{
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  
  // Query for needed interfaces.
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
  
  osg::Vec3d location ( this->location() );

  // Make new extents.
  Extents e ( osg::Vec2d ( location[0], location[1] ), osg::Vec2d ( location[0], location[1] ) );
  this->extents ( e );

  // Get the height.
  const double height ( this->_elevation ( location, elevation.get() ) );

  osg::Vec3d hpr ( this->orientation() );
  const double heading ( hpr[0] ), tilt ( hpr[1] ), roll ( hpr[2] );
  
  osg::Matrixd R ( planet.valid() ? planet->planetRotationMatrix ( location[1], location[0], height, heading ) : osg::Matrixd() );
  osg::Matrixd S ( osg::Matrixd::scale ( this->scale() * this->toMeters() ) );

  mt->setMatrix ( S *
                  osg::Matrix::rotate ( osg::DegreesToRadians ( tilt ), osg::Vec3 ( 1.0, 0.0, 0.0 ) ) * 
                  osg::Matrix::rotate ( osg::DegreesToRadians ( roll ), osg::Vec3 ( 0.0, 1.0, 0.0 ) ) * R );

  if ( true == this->optimize() )
  {
    osgUtil::Optimizer optimizer;
    optimizer.optimize ( mt.get(), osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS );
  }

  // If there is a scale, turn on normalize.
  if ( false == S.isIdentity() )
    OsgTools::State::StateSet::setNormalize ( this->model(), true );

  mt->addChild ( this->model() );
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the location.
//
///////////////////////////////////////////////////////////////////////////////

void Model::location ( const osg::Vec3d& location )
{
  Guard guard ( this->mutex() );
  _location = location;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the location.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3d Model::location() const
{
  Guard guard ( this->mutex() );
  return _location;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the orientation.
//
///////////////////////////////////////////////////////////////////////////////

void Model::orientation( double heading, double  tilt, double  roll )
{
  Guard guard ( this->mutex() );
  _heading = heading;
  _tilt = tilt;
  _roll = roll;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the orientation.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3d Model::orientation() const
{
  osg::Vec3d v;
  {
    Guard guard ( this->mutex() );
    v[0] = _heading;
    v[1] = _tilt;
    v[2] = _roll;
  }
  return v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scale.
//
///////////////////////////////////////////////////////////////////////////////

void Model::scale ( const osg::Vec3d& scale )
{
  Guard guard ( this->mutex() );
  _scale = scale;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scale.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3d Model::scale() const
{
  Guard guard ( this->mutex() );
  return _scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model.
//
///////////////////////////////////////////////////////////////////////////////

void Model::model ( osg::Node* node )
{
  Guard guard ( this->mutex() );
  _model = node;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Model::model() const
{
  Guard guard ( this->mutex() );
  return _model.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scale to convert to meters.
//
///////////////////////////////////////////////////////////////////////////////

void Model::toMeters ( double scale )
{
  Guard guard ( this->mutex() );
  _toMeters = scale;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scale to convert to meters.
//
///////////////////////////////////////////////////////////////////////////////

double Model::toMeters() const
{
  Guard guard ( this->mutex() );
  return _toMeters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the optimization flag.
//
///////////////////////////////////////////////////////////////////////////////

void Model::optimize ( bool state )
{
  Guard guard ( this->mutex() );
  _optimize = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the optimization flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Model::optimize() const
{
  Guard guard ( this->mutex() );
  return _optimize;
}
