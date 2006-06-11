
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  An OpenGL rendering context.
//
///////////////////////////////////////////////////////////////////////////////

#include "DotNet/Glue/RenderContext.h"
#include "DotNet/Glue/PixelFormat.h"

#include "Usul/System/LastError.h"
#include "Usul/Errors/Assert.h"

#include <sstream>

using namespace CadKit::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RenderContext::RenderContext ( System::Windows::Forms::Control ^control, unsigned int pixelFormat ) : BaseClass(),
  _control     ( control ),
  _dc          ( gcnew DeviceContext ( control ) ),
  _rc          ( 0x0 ),
  _pixelFormat ( pixelFormat )
{
  if ( nullptr == control )
    throw gcnew System::ArgumentException ( "Error 3004475898: Invalid control handle given" );

  if ( false == control->IsHandleCreated )
    throw gcnew System::ArgumentException ( "Error 4283634847: Control's window handle not created" );

  _dc->reference();

  try
  {
    this->_create();
  }

  catch ( ... )
  {
    this->_cleanup();
    throw;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RenderContext::~RenderContext()
{
  System::Diagnostics::Debug::Assert ( nullptr == _control );
  System::Diagnostics::Debug::Assert ( nullptr == _dc );
  System::Diagnostics::Debug::Assert ( 0x0 == _rc );
  System::Diagnostics::Debug::Assert ( true == this->IsClean );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dispose the resources.
//
///////////////////////////////////////////////////////////////////////////////

void RenderContext::_cleanup()
{
  try
  {
    this->_delete();
  }
  catch ( System::Exception ^e )
  {
    System::Console::WriteLine ( "Error 3940735851: ", e->Message );
  }
  catch ( const std::exception &e )
  {
    if ( 0x0 != e.what() )
    {
      System::Console::WriteLine ( "Error 2723660777: ", gcnew System::String ( e.what() ) );
    }
    else
    {
      System::Console::WriteLine ( "Error 1762524380: Standard exception caught when deleting rendering context" );
    }
  }
  catch ( ... )
  {
    System::Console::WriteLine ( "Error 2446582359: Unknown exception caught when deleting rendering context" );
  }
  finally
  {
    BaseClass::_cleanup();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Delete the existing OpenGL rendering context.
//
///////////////////////////////////////////////////////////////////////////////

void RenderContext::_delete()
{
  _control = nullptr;
  _pixelFormat = 0;

  DeviceContext ^dc = _dc;
  HGLRC rc = _rc;

  _dc = nullptr;
  _rc = 0x0;

  if ( nullptr != dc )
    dc->dereference();

  if ( 0x0 != rc )
  {
    Usul::System::LastError::init();
    if ( FALSE == ::wglDeleteContext ( rc ) )
    {
      std::ostringstream out;
      out << "Error 1474302060: Failed to delete OpenGL render context.";
      if ( Usul::System::LastError::has() )
      {
        out << " System Error: " << Usul::System::LastError::message();
      }
      throw gcnew System::Exception ( gcnew System::String ( out.str().c_str() ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Create the OpenGL rendering context.
//
///////////////////////////////////////////////////////////////////////////////

void RenderContext::_create()
{
  System::Diagnostics::Debug::Assert ( 0x0 == _rc );
  System::Diagnostics::Debug::Assert ( nullptr != _dc );

  PixelFormat ^pf = nullptr;

  try
  {
    // This will try to choose the given index, and fall back on the "best" one.
    // It also sets the final format.
    pf = gcnew PixelFormat ( _dc, _pixelFormat );
    pf->reference();
    _pixelFormat = pf->index();

    // Create the context.
    Usul::System::LastError::init();
    _rc = ::wglCreateContext ( _dc->hdc() );
    if ( 0 == _rc )
    {
      std::ostringstream out;
      out << "Error 4090027866: Failed to create OpenGL render context.";
      if ( Usul::System::LastError::has() )
      {
        out << " System Error: " << Usul::System::LastError::message();
        out << " Pixel Format: " << ::GetPixelFormat ( _dc->hdc() );
      }
      throw gcnew System::Exception ( gcnew System::String ( out.str().c_str() ) );
    }
  }

  finally
  {
    if ( nullptr != pf )
    {
      pf->dereference();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the rendering context valid?
//
///////////////////////////////////////////////////////////////////////////////

bool RenderContext::valid()
{
  return ( nullptr != _control && 
              true == _control->IsHandleCreated &&
                 0 != _rc &&
           nullptr != _dc );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the pixel format.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int RenderContext::pixelFormat()
{
  return _pixelFormat;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the device context.
//
///////////////////////////////////////////////////////////////////////////////

DeviceContext ^RenderContext::deviceContext()
{
  return _dc;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the rendering context current.
//
///////////////////////////////////////////////////////////////////////////////

void RenderContext::makeCurrent()
{
  System::Diagnostics::Debug::Assert ( nullptr != _dc );
  System::Diagnostics::Debug::Assert ( 0x0 != _rc );

  if ( FALSE == ::wglMakeCurrent ( _dc->hdc(), _rc ) )
    throw gcnew System::Exception ( "Error 1975756094: Failed to make OpenGL rendering context current" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swap the buffers.
//
///////////////////////////////////////////////////////////////////////////////

void RenderContext::swapBuffers()
{
  System::Diagnostics::Debug::Assert ( nullptr != _dc );

  if ( FALSE == ::SwapBuffers ( _dc->hdc() ) )
    throw gcnew System::Exception ( "Error 3542185368: Failed to swap buffers" );
}
