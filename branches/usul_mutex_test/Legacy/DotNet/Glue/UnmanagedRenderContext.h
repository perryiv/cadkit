
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  An unmanged OpenGL rendering context.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IOpenGlContext.h"

namespace CadKit
{
  namespace OpenGL
  {
    namespace Glue
    {
      typedef void (*managedCallback) (void);

      class UnmanagedRenderContext : public Usul::Base::Referenced,
                                     public Usul::Interfaces::IOpenGLContext
      {
      public:
        typedef Usul::Base::Referenced BaseClass;

        // Smart-pointer definitions.
        USUL_DECLARE_QUERY_POINTERS ( UnmanagedRenderContext );

        // Usul::Interfaces::IUnknown members.
        USUL_DECLARE_IUNKNOWN_MEMBERS;

        UnmanagedRenderContext();
        virtual ~UnmanagedRenderContext();

        virtual bool    isContextThread() const;
        virtual void    makeCurrent();
        virtual void    swapBuffers();

        void            makeCurrentCallback( managedCallback callback );
        void            swapBuffersCallback( managedCallback callback );

      private:
        managedCallback _makeCurrentCallback;
        managedCallback _swapBuffersCallback;
        unsigned long   _threadId;
      };
    }
  }
}
