
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Commands/Camera.h"

using namespace VRV::Commands;

USUL_IMPLEMENT_COMMAND ( Camera );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Camera::Camera ( const std::string& name, CameraOption option, Usul::Interfaces::IUnknown *caller ) : 
BaseClass ( caller ),
_option ( option )
{
  this->text ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Camera::~Camera ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void Camera::_execute ()
{
  Usul::Interfaces::ICamera::QueryPtr c ( this->caller () );

  if ( c.valid () )
    c->camera ( _option );
}