
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __HIDESHOWNORMALSCOMPONENT_H__
#define __HIDESHOWNORMALSCOMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/GUI/IMenuEntry.h"

class HideShowNormalsComponent : public Usul::Base::Referenced,
                                 public Usul::Interfaces::ICommand,
                                 public Usul::Interfaces::IMenuEntry,
                                 public Usul::Interfaces::IPlugin
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HideShowNormalsComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  HideShowNormalsComponent();

protected:    
  /// Destructor
  ~HideShowNormalsComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const;
    
  /// Usul::Interfaces::ICommand
  virtual void execute ( Usul::Interfaces::IUnknown *caller );

  /// Usul::Interfaces::IMenuEntry
  virtual std::string   menuText() const;
  virtual std::string   hotKeyText() const;
  virtual std::string   statusBarText() const;
  virtual std::string   menuGroup() const;
};

#endif /* __HIDESHOWNORMALSCOMPONENT_H__  */		
	
