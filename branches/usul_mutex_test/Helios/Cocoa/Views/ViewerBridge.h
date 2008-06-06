///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __ViewerBridge_H
#define __ViewerBridge_H 

#if __OBJC__

#import <Cocoa/Cocoa.h>

#include "Usul/Interfaces/ITimeoutSpin.h"
#include "Usul/Interfaces/ISpin.h"
#include "Usul/Interfaces/ITimeoutAnimate.h"
#include "Usul/Base/Referenced.h"

#include "Helios/Cocoa/Views/OSGView.h"

namespace Helios {
  namespace Cocoa {
    namespace Views {
      
      
class ViewerBridge :  public Usul::Interfaces::ITimeoutSpin,
                      public Usul::Interfaces::ITimeoutAnimate,
                      public Usul::Base::Referenced
{
  public:
  
  typedef Usul::Base::Referenced BaseClass;
  /// Smart-pointer definitions. - Needed because of Multiple Inheritence
  USUL_DECLARE_REF_POINTERS ( ViewerBridge );
  
  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  ViewerBridge(OSGView *osgView);
  
// -----------------------------------------------------------------------------
//    ITimeoutSpin Interface
// -----------------------------------------------------------------------------
  void stopSpin ( );
  void startSpin ( double timeout );
  
// -----------------------------------------------------------------------------
//    ITimeoutAnimate Interface
// -----------------------------------------------------------------------------
  void startAnimation ( double interval );
  
  
  protected:
    ~ViewerBridge();  /// Protect the Destructor so Ref Counting Works
  
  private:
    OSGView *_osgView;


};

    } // end Views Namespace
  } //end Cocoa namespace
} //end Helios namespace

#endif  // Cocoa only code

#endif