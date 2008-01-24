
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

#include "Usul/Interfaces/IPlanetCoordinates.h"

#include "osg/MatrixTransform"

#include "ossim/base/ossimGpt.h"
#include <ossim/base/ossimLsrSpace.h>
#include <ossim/base/ossimMatrix4x4.h>

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
  
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
  
  osg::Vec3 translate ( this->location() );
  
  if( planet.valid() )
  {
    Usul::Math::Vec3d p ( translate[0], translate[1], translate[2] );
    planet->convertToPlanet( Usul::Math::Vec3d ( p ), p );
    translate.set ( p[0], p[1], p[2] );
  }
  
  double heading ( 0.0 ), tilt ( 0.0 ), roll ( 0.0 );
  this->orientation( heading, tilt, roll );
  
  osg::Matrix T ( osg::Matrix::translate ( translate ) );
  osg::Matrix R ( osg::Matrix::rotate ( osg::DegreesToRadians ( tilt ), osg::Vec3 ( 1.0, 0.0, 0.0 ),
                                        osg::DegreesToRadians ( roll ), osg::Vec3 ( 0.0, 1.0, 0.0 ),
                                        osg::DegreesToRadians ( heading ), osg::Vec3 ( 0.0, 0.0, 1.0 ) ) );
  osg::Matrix S ( osg::Matrix::scale ( this->scale() * 0.0254 ) );
  
#if 0
  osg::Vec3 normal ( translate );
  normal.normalize();
  osg::Matrix R1 ( osg::Matrix::rotate ( osg::Vec3 ( 0.0, 0.0, 1.0 ), normal ) );
  mt->setMatrix ( R1 * S * R * T );
#else
  double sin_lat = ::sin ( this->location()[1] );
  double cos_lat = ::cos ( this->location()[1] );
  double sin_lon = ::sin( this->location()[0] );
  double cos_lon = ::cos( this->location()[0] );
  
  Usul::Math::Vec3d E (-sin_lon,
                         cos_lon,
                         0.0);
  Usul::Math::Vec3d N (-sin_lat*cos_lon,
                         -sin_lat*sin_lon,
                         cos_lat);
  Usul::Math::Vec3d U (E.cross(N));
  
#if 0
  osg::Matrix R1 ( E[0], E[1], E[2], 0.0,
                   N[0], N[1], N[2], 0.0,
                   U[0], U[1], U[2], 0.0,
                   translate[0], translate[1], translate[2], 1.0);
#else
  /*osg::Matrix R1 ( E[0], N[0], U[0], 0.0,
                   E[1], N[1], U[1], 0.0,
                   E[2], N[2], U[2], 0.0,
                   translate[0], translate[1], translate[2], 1.0);*/
  osg::Vec3d xyz ( translate );
  ossimLsrSpace lsrSpace (ossimGpt( this->location()[1],
                                     this->location()[0],
                                     this->location()[2]), 0.0);
  
  ossimMatrix4x4 lsrMatrix(lsrSpace.lsrToEcefRotMatrix());
  NEWMAT::Matrix compositeMatrix = lsrMatrix.getData();
  /*       compositeMatrix = compositeMatrix.t(); */
  osg::Matrix R1 = osg::Matrixd(compositeMatrix[0][0], compositeMatrix[1][0], compositeMatrix[2][0], 0.0,
                        compositeMatrix[0][1], compositeMatrix[1][1], compositeMatrix[2][1], 0.0,
                        compositeMatrix[0][2], compositeMatrix[1][2], compositeMatrix[2][2], 0.0,
                        xyz[0], xyz[1], xyz[2], 1.0);
  
  
#endif
  mt->setMatrix (  S * R1 );
#endif
  
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
