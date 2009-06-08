
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_COMPONENT_CLASS_H_
#define _WX_GUI_COMPONENT_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"
#include "AppFrameWork/Core/Program.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IChangeMenuBar.h"
#include "Usul/Interfaces/IGuiInit.h"


class WxComponent : public Usul::Base::Referenced,
                    public Usul::Interfaces::IPlugin,
                    public Usul::Interfaces::IChangeMenuBar,
                    public Usul::Interfaces::IGuiInit
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;
  typedef AFW::Core::Program::Factories Factories;

  // Type information.
  USUL_DECLARE_TYPE_ID ( WxComponent );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( WxComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  WxComponent();

  // Change to menu bar.
  virtual void                  changeMenuBar ( IUnknown *caller );

  // Initialize the gui.
  virtual void                  guiInit ( Usul::Interfaces::IUnknown *caller );

  // Return name of plugin.
  virtual std::string           getPluginName() const;

protected: 

  // Do not copy.
  WxComponent ( const WxComponent & );
  WxComponent &operator = ( const WxComponent & );

  // Use reference counting.
  virtual ~WxComponent();

  void                          _registerFactory ( const std::type_info &info, AFW::Core::NewObjectFunctor *functor );

  Usul::Interfaces::IUnknown *  _unknown();

private:

  mutable Mutex _mutex;
  Factories _original;
};


#endif // _WX_GUI_COMPONENT_CLASS_H_
