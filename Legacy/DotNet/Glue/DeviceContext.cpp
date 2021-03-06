
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A device context.
//
///////////////////////////////////////////////////////////////////////////////

#include "DotNet/Glue/DeviceContext.h"

#include "Usul/Cast/Cast.h"

using namespace CadKit::OpenGL::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DeviceContext::DeviceContext ( System::Windows::Forms::Control ^control ) : BaseClass(),
  _window ( 0x0 ),
  _hdc ( 0x0 )
{
  if ( nullptr == control )
    throw gcnew System::ArgumentException ( "Error 3320039180: Invalid control handle given" );

  if ( false == control->IsHandleCreated )
    throw gcnew System::ArgumentException ( "Error 1225722070: Control's window handle not created" );

  _window = USUL_UNSAFE_CAST ( HWND, control->Handle.ToPointer() ) ;
  _hdc = ::GetDC ( _window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DeviceContext::~DeviceContext()
{
  System::Diagnostics::Debug::Assert ( 0x0 == _window );
  System::Diagnostics::Debug::Assert ( 0x0 == _hdc );
  System::Diagnostics::Debug::Assert ( true == this->IsClean );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up the resources.
//
///////////////////////////////////////////////////////////////////////////////

void DeviceContext::_cleanup()
{
  HWND window ( _window );
  HDC hdc ( _hdc ); 
  _window = 0x0;
  _hdc = 0x0;
  if ( 0x0 != window && 0x0 != hdc )
    ::ReleaseDC ( window, hdc );
  BaseClass::_cleanup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the rendering context.
//
///////////////////////////////////////////////////////////////////////////////

HDC DeviceContext::hdc()
{
  return _hdc;
}
