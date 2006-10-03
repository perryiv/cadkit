
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

#pragma once

#include "DotNet/Glue/DeviceContext.h"

#include "Usul/Headers/OpenGL.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "windows.h"


namespace CadKit
{
  namespace OpenGL
  {
    namespace Glue
    {
      public ref class RenderContext : CadKit::Referenced::Base
      {
      public:

        typedef CadKit::Referenced::Base BaseClass;

        RenderContext ( System::Windows::Forms::Control ^control, unsigned int pixelFormat );

        DeviceContext ^     deviceContext();
        void                makeCurrent();
        unsigned int        pixelFormat();
        void                swapBuffers();
        bool                valid();

      protected:

        virtual void        _cleanup() override;
        void                _create();
        void                _delete();

      private:

        virtual ~RenderContext();

        System::Windows::Forms::Control ^_control;
        DeviceContext ^_dc;
        HGLRC _rc;
        unsigned int _pixelFormat;
      };
    }
  }
}
