
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/DataObjects/Model.h"
#include "Minerva/Core/Visitor.h"

#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"

#include "osg/MatrixTransform"

using namespace Minerva::Core::DataObjects;


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
  _scale ( 1.0, 1.0, 1.0 ),
  _model ( 0x0 )
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
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Model::accept ( Minerva::Core::Visitor& visitor )
{
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Model::_preBuildScene( Usul::Interfaces::IUnknown* caller )
{
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  
  // Query for needed interfaces.
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
  
  osg::Vec3 location ( this->location() );

  // Make new extents.
  Extents e ( osg::Vec2d ( location[0], location[1] ), osg::Vec2d ( location[0], location[1] ) );
  this->extents ( e );

  // Get the height.
  const double height ( this->_elevation ( location, elevation.get() ) );

  double heading ( 0.0 ), tilt ( 0.0 ), roll ( 0.0 );
  this->orientation( heading, tilt, roll );
  
  osg::Matrixd R ( planet.valid() ? planet->planetRotationMatrix ( location[1], location[0], height, heading ) : osg::Matrixd() );
  osg::Matrix S ( osg::Matrix::scale ( this->scale() * 0.0254 ) );
  
  mt->setMatrix ( S *
                  osg::Matrix::rotate ( osg::DegreesToRadians ( tilt ), osg::Vec3 ( 1.0, 0.0, 0.0 ) ) * 
                  osg::Matrix::rotate ( osg::DegreesToRadians ( roll ), osg::Vec3 ( 0.0, 1.0, 0.0 ) ) * R );
  
  mt->addChild ( this->model() );
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the location.
//
///////////////////////////////////////////////////////////////////////////////

void Model::location ( const osg::Vec3& location )
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

osg::Vec3 Model::location() const
{
  Guard guard ( this->mutex() );
  return _location;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the orientation.
//
///////////////////////////////////////////////////////////////////////////////

void Model::orientation( double  heading, double  tilt, double  roll )
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

void Model::orientation( double& heading, double& tilt, double &roll ) const
{
  Guard guard ( this->mutex() );
  heading = _heading;
  tilt = _tilt;
  roll = _roll;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scale.
//
///////////////////////////////////////////////////////////////////////////////

void Model::scale ( const osg::Vec3& scale )
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

osg::Vec3 Model::scale() const
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
