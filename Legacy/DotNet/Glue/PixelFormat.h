
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Pixel format class.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DotNet/Glue/DeviceContext.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "windows.h"

#include <vector>
#include <string>


namespace CadKit
{
  namespace OpenGL
  {
    namespace Glue
    {
      public ref class PixelFormat : CadKit::Referenced::Base
      {
      public:

        typedef CadKit::Referenced::Base BaseClass;
        typedef PIXELFORMATDESCRIPTOR Descriptor;
        typedef std::vector<std::string> Row;
        typedef std::vector<Row> Table;
        typedef System::Data::DataTable DataTable;

        PixelFormat ( DeviceContext ^dc, unsigned int index );

        const Descriptor &    descriptor();
        unsigned int          index();
        static void           query ( DeviceContext ^dc, Table & );
        static DataTable ^    query ( DeviceContext ^dc );

      protected:

        virtual void          _cleanup() override;
        int                   _default();
        void                  _init();
        int                   _lastIndex();
        void                  _set();

      private:

        virtual ~PixelFormat();

        Descriptor *_pfd;
        int _index;
        DeviceContext ^_dc;
      };
    }
  }
}
