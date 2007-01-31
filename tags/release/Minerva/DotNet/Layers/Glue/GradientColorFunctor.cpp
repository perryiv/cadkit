
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "GradientColorFunctor.h"

#include "Minerva/Layers/Functors/GradientColorFunctor.h"

using namespace DT::Minerva::Glue;

GradientColorFunctor::GradientColorFunctor() : 
_minColor ( nullptr ),
_maxColor ( nullptr ),
_minValue ( 0.0 ),
_maxValue ( 0.0 )
{
}

Minerva::Layers::Functors::BaseColorFunctor * GradientColorFunctor::colorFunctor()
{
  ::Minerva::Layers::Functors::GradientColorFunctor::RefPtr functor ( new ::Minerva::Layers::Functors::GradientColorFunctor );

  functor->minColor( this->toOsgColor( _minColor ) );
  functor->maxColor( this->toOsgColor( _maxColor ) );
  functor->minValue( _minValue );
  functor->maxValue( _maxValue );

  return functor.release();
}
