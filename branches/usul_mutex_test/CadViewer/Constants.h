
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Constants: Constant expressions and values.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_CONSTANTS_H_
#define _CV_CONSTANTS_H_

#include "Usul/Math/Vector4.h"


namespace CV {


// Defaults.
const float               DEFAULT_MAX_RELATIVE_SPEED      ( 0.05f ); // percent / second
const float               DEFAULT_MAX_ANGULAR_SPEED       ( 30 );    // degrees / second
const Usul::Math::Vec4f   DEFAULT_RAY_COLOR               ( 1, 0, 0, 1 );
const float               DEFAULT_RAY_THICKNESS           ( 2 );

// Messages.
const std::string         NOT_APP_THREAD ( "This part of the program should only be executed from the application thread." );

}; // namespace CV


#endif // _CV_CONSTANTS_H_
