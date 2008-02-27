
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Transform class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Transform.h"

#include "OsgTools/Convert.h"

#include "osg/MatrixTransform"

using namespace SAL;
using namespace SAL::OSG;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Transform, Transform::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Transform::Transform ( osg::MatrixTransform *mt ) : BaseClass ( mt ),
  _mt ( mt )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Transform::~Transform()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Transform::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case SAL::Interfaces::ITransformFloat::IID:
    return static_cast<SAL::Interfaces::ITransformFloat*>(this);
  case SAL::Interfaces::ITransformDouble::IID:
    return static_cast<SAL::Interfaces::ITransformDouble*>(this);
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Transform::getTransformMatrix ( Matrixf &to ) const
{
  const osg::Matrix &from = _mt->getMatrix();
  OsgTools::Convert::matrix ( from, to );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Transform::setTransformMatrix ( const Matrixf &from )
{
  osg::Matrix to;
  OsgTools::Convert::matrix ( from, to );
  _mt->setMatrix ( to );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Transform::getTransformMatrix ( Matrixd &to ) const
{
  const osg::Matrix &from = _mt->getMatrix();
  OsgTools::Convert::matrix ( from, to );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Transform::setTransformMatrix ( const Matrixd &from )
{
  osg::Matrix to;
  OsgTools::Convert::matrix ( from, to );
  _mt->setMatrix ( to );
}