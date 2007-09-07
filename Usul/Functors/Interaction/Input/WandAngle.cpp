
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base functor class for wand-angle input.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functors/Interaction/Input/WandAngle.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Functors::Interaction::Input;

USUL_IMPLEMENT_TYPE_ID ( WandAngle );


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

WandAngle::WandAngle ( Unknown *caller, const std::string &name ) : 
  BaseClass ( caller, name ),
  _wand ( caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

WandAngle::WandAngle ( const WandAngle &wp ) : BaseClass ( wp ),
  _wand ( wp._wand )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WandAngle::~WandAngle()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the value. 
//
///////////////////////////////////////////////////////////////////////////////

float WandAngle::_calculateValue ( unsigned int zero )
{
  USUL_TRACE_SCOPE;
  float value ( 0.0f );

  if ( true == _wand.valid() )
  {
    IWandState::Matrix matrix;
    {
      Guard guard ( this->mutex() );
      _wand->wandRotation ( matrix );
    }

    IWandState::Vector nz ( 0.0f, 0.0f, -1.0f );
    IWandState::Vector localDir ( nz );
    IWandState::Vector globalDir = matrix * localDir;

    // Lose the specified coordinate.
    globalDir[zero] = 0.0f;

    value = globalDir.angle ( nz );
    value *= Usul::Math::RAD_TO_DEG;
    value = ( ( _value < 0.0f ) ? ( _value + 360.0f ) : _value ); // [0,360]
    value = Usul::Math::minimum ( _value, 360.0f ); // [0,360]
    value = Usul::Math::maximum ( _value, 0.0f );   // [0,360]
    value -= -180.0f; // [-180,180];
  }

  return value;
}
