
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

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "windows.h"


namespace CadKit
{
  namespace OpenGL
  {
    namespace Glue
    {
      public ref class DeviceContext : CadKit::Referenced::Base
      {
      public:

        typedef CadKit::Referenced::Base BaseClass;

        DeviceContext ( System::Windows::Forms::Control ^ );

        HDC                 hdc();

      protected:

        virtual void        _cleanup() override;

      private:

        virtual ~DeviceContext();

        HWND _window;
        HDC _hdc;
      };
    }
  }
}
