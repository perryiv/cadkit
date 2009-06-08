
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CocoaGLContext_H
#define __CocoaGLContext_H 

#if __OBJC__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IOpenGLContext.h" 

#import <Cocoa/Cocoa.h>

namespace Helios {
namespace Cocoa {
namespace Views  {
    
  
class CocoaGLContext : public Usul::Base::Referenced,
                       public Usul::Interfaces::IOpenGLContext
{
public:
  typedef Usul::Base::Referenced BaseClass;
  
  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  CocoaGLContext( NSOpenGLContext *context);
  virtual ~CocoaGLContext();
  
  void    makeCurrent();
  void    swapBuffers();
	bool    isContextThread() const;

private:
  NSOpenGLContext *_context;
};

} // end Views Namespace
} //end Cocoa namespace
} //end Helios namespace


#endif  // Cocoa only code


#endif  //__CocoaGLContext_H
