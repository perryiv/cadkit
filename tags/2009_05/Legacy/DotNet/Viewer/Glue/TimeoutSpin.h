
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Proxy between OsgTools::Render::Viewer and CadKit::Viewer::Glue::Viewer.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_VIEWER_GLUE_TIMEOUT_SPIN_H__
#define __CADKIT_VIEWER_GLUE_TIMEOUT_SPIN_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/ITimeoutSpin.h"


namespace OsgTools { namespace Render { class Viewer; } }


namespace CadKit {
namespace Viewer {
namespace Glue {


class TimeoutSpin : public Usul::Base::Referenced,
                    public Usul::Interfaces::ITimeoutSpin
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef void (*TimeoutCallback) ( bool start, double span );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TimeoutSpin );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor.
  TimeoutSpin();

  /// Set time-out callback.
  void                  timeoutCallback ( TimeoutCallback cb ) { _timeout = cb; }

protected:

  virtual ~TimeoutSpin();

  /// Usul::Interfaces::ITimeoutSpin
  virtual void          stopSpin();
  virtual void          startSpin ( double span );

private:

  TimeoutCallback _timeout;
};


}
}
}


#endif // __CADKIT_VIEWER_GLUE_TIMEOUT_SPIN_H__
