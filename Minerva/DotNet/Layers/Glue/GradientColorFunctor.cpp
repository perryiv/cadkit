
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "GradientColorFunctor.h"

#include "Minerva/Core/Functors/GradientColorFunctor.h"

using namespace DT::Minerva::Glue;

GradientColorFunctor::GradientColorFunctor() : 
_minColor ( nullptr ),
_maxColor ( nullptr ),
_minValue ( 0.0 ),
_maxValue ( 0.0 )
{
}

Minerva::Core::Functors::BaseColorFunctor * GradientColorFunctor::colorFunctor()
{
  ::Minerva::Core::Functors::GradientColorFunctor::RefPtr functor ( new ::Minerva::Core::Functors::GradientColorFunctor );

  functor->minColor( this->toOsgColor( _minColor ) );
  functor->maxColor( this->toOsgColor( _maxColor ) );
  functor->minValue( _minValue );
  functor->maxValue( _maxValue );

  return functor.release();
}
