
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
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Absolute.h"


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

double WandAngle::_calculateValue ( unsigned int zero, unsigned int sign, const Vector& dir )
{
  USUL_TRACE_SCOPE;
  double value ( 0.0 );

  if ( true == _wand.valid() )
  {
    IWandState::Matrix matrix;
    {
      Guard guard ( this->mutex() );
      _wand->wandRotation ( matrix );
    }

    //std::cout << "---------------------------------------------------";
    //Usul::Print::matrix ( "", matrix.get(), std::cout );
    //std::cout << std::endl;
    //IWandState::Vector nz ( 0.0f, 0.0f, -1.0f );
    IWandState::Vector localDir ( dir );
    IWandState::Vector globalDir = matrix * localDir;

    // Lose the specified coordinate.
    globalDir[zero] = 0;

    value = globalDir.angle ( dir );
    value *= Usul::Math::RAD_TO_DEG;
    value = Usul::Math::absolute ( value );
    value *= ( ( globalDir[sign] < 0 ) ? -1 : 1 );
    value = Usul::Math::clamp ( value, -90.0, 90.0 );
    value += 90;  // [0,180]
    value /= 180; // [0,1];
    value *= ( _range[1] - _range[0] );
    value = _range[0] + value; // [ _range[0], _range[1] ]

#if 0
    if ( 0 == zero )
    {    
      std::cout << "Angle: " << std::setw ( 8 ) << static_cast < int > ( value ) << std::endl;
    }
#endif
  }

  return value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the caller.
//
///////////////////////////////////////////////////////////////////////////////

void WandAngle::caller ( Unknown* caller )
{
  USUL_TRACE_SCOPE;

  // Call the base class.
  BaseClass::caller ( caller );

  // Set our internal query pointer to caller.
  Guard guard ( this->mutex() );
  _wand = caller;
}
