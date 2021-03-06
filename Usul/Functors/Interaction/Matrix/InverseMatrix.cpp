
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Inverse matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functors/Interaction/Matrix/InverseMatrix.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Functors::Interaction::Matrix;

USUL_IMPLEMENT_TYPE_ID ( InverseMatrix );


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructors.
//
///////////////////////////////////////////////////////////////////////////////

InverseMatrix::InverseMatrix ( Unknown *caller, const std::string &name, BaseClass *mf ) : 
  BaseClass ( caller, name ), 
  _mf ( mf )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructors.
//
///////////////////////////////////////////////////////////////////////////////

InverseMatrix::InverseMatrix ( const InverseMatrix &cb ) : 
  BaseClass ( cb ), 
  _mf ( cb._mf )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructors.
//
///////////////////////////////////////////////////////////////////////////////

InverseMatrix::~InverseMatrix()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the inverse of the contained matrix.
//
///////////////////////////////////////////////////////////////////////////////

void InverseMatrix::operator()()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( true == _mf.valid() )
  {
    (*_mf)();
    _matrix = _mf->matrix();
    _matrix.invert();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Functors::Interaction::Common::BaseFunctor* InverseMatrix::clone()
{
  return new InverseMatrix ( *this );
}
