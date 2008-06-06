
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_APPLICATION_H_
#define _FOX_TOOLS_APPLICATION_H_

#include "FoxTools/Export/Export.h"

#include "FoxTools/Headers/App.h"

#include <string>


namespace FoxTools {
namespace App {

class FOX_TOOLS_EXPORT Application : public FX::FXApp
{
public:

  // Useful typedefs.
  typedef FX::FXApp BaseClass;

  // Message ids.
  enum 
  {
    ID_DROP_FILE = BaseClass::ID_LAST,
    ID_LAST
  };

  // Constructor, destructor.
  Application ( const std::string &name = "OSG FOX Application", const std::string &vendor = "CadKit" );
  virtual ~Application();

  // Get the level we are at in the event queue.
  unsigned int eventQueueLevel() const { return _eventQueueLevel; }

protected:

  // Handles all events.
#ifdef _MSC_VER
  virtual long         dispatchEvent ( FX::FXID hwnd, unsigned int message, unsigned int wParam, long lParam );
  long                _dispatchEvent ( FX::FXID hwnd, unsigned int message, unsigned int wParam, long lParam );
#else
  virtual FX::FXbool   dispatchEvent ( FXRawEvent& ev );
  FX::FXbool          _dispatchEvent ( FXRawEvent& ev );
#endif

  // Handle drop-event.
  long                _handleDropEvent ( FX::FXID, unsigned int, unsigned int, long );

  // Report any errors.
  void                _reportErrors() const;

private:

  unsigned int _eventQueueLevel;

  FXDECLARE ( Application );
};

} // namespace App
} // namespace FoxTools


#endif //_FOX_TOOLS_APPLICATION_H_
