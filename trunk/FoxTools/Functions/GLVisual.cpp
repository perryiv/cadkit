
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get and set pointer to the OpenGL visual.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Functions/GLVisual.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Headers/GLVisual.h"

#include "Usul/Errors/Assert.h"

#include <string>
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Pointer to the visual.
//
///////////////////////////////////////////////////////////////////////////////

namespace FoxTools
{
  namespace Functions
  {
    namespace Detail
    {
      FX::FXGLVisual *_visual ( 0x0 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get pointer to the visual. Create if necessary.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXGLVisual *FoxTools::Functions::visual()
{
  // If the app is null then there is no visual, even if our pointer may be 
  // non-null. This will happen when the application goes out of scope.
  FX::FXApp *app ( FoxTools::Functions::application() );
  if ( 0x0 == app )
  {
    std::string error ( "Error 2052657400: Accessing visual when application pointer is null too" );
    throw std::runtime_error ( error );
  }

  // There is an app, so if not visual, create it.
  if ( 0x0 == Detail::_visual )
  {
    Detail::_visual = new FX::FXGLVisual ( app, FX::VISUAL_DOUBLEBUFFER );
    if ( 0x0 == Detail::_visual )
    {
      std::string error ( "Error 3008872053: Allocation of visual failed" );
      throw std::runtime_error ( error );
    }
  }

  // If we get to here then return the visual.
  USUL_ASSERT ( 0x0 != Detail::_visual );
  return Detail::_visual;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set pointer to the visual.
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Functions::visual ( FX::FXGLVisual *v )
{
  Detail::_visual = v;
}
