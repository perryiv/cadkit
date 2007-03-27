
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base callback class for functors that transform the scene.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "MatrixFunctor.h"

using namespace CV;
using namespace CV::Functors;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

MatrixFunctor::MatrixFunctor ( Unknown *unknown ) : 
  BaseClass ( unknown, 1075949546 ),
  _matrix()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructors.
//
///////////////////////////////////////////////////////////////////////////////

MatrixFunctor::MatrixFunctor ( const MatrixFunctor &cb ) : 
  BaseClass ( cb ),
  _matrix ( cb._matrix )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructors.
//
///////////////////////////////////////////////////////////////////////////////

MatrixFunctor::~MatrixFunctor()
{
}
