
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IOpenGLContext_H
#define __IOpenGLContext_H 

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {
    
    
struct IOpenGLContext : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOpenGLContext );
  
  /// Id for this interface.
  enum { IID = 2489692458u };
  
  virtual bool    isContextThread() const = 0;
  virtual void    makeCurrent()  = 0;
  virtual void    swapBuffers()  = 0;
    
}; // class IOpenGLContext
    

} // namespace Interfaces
} // namespace Usul


#endif
