
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Plugin for the text-message window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MESSAGE_WINDOWSCOMPONENT_H__
#define __MESSAGE_WINDOWSCOMPONENT_H__

#include "MessageWindows/CompileGuard.h"

#include "AppFrameWork/Windows/TextWindow.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IChangeMenuBar.h"
#include "Usul/Interfaces/IGuiInit.h"


class MessageWindowsComponent : public Usul::Base::Referenced,
                                public Usul::Interfaces::IPlugin,
                                public Usul::Interfaces::IChangeMenuBar,
                                public Usul::Interfaces::IGuiInit
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MessageWindowsComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  // Constructor
  MessageWindowsComponent();

protected:    

  // Destructor
  ~MessageWindowsComponent();

  // Change to menu bar.
  virtual void                  changeMenuBar ( Usul::Interfaces::IUnknown *caller );

  // Initialize the gui.
  virtual void                  guiInit ( Usul::Interfaces::IUnknown *caller );

  // Get plugin name.
  virtual std::string           getPluginName() const;

private:

  mutable Mutex _mutex;
  AFW::Windows::TextWindow::RefPtr _textWindow;
};

#endif // __MESSAGE_WINDOWSCOMPONENT_H__
