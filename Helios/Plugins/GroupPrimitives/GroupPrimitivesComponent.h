
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

#ifndef __GROUPPRIMITIVESCOMPONENT_H__
#define __GROUPPRIMITIVESCOMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/GUI/IMenuEntry.h"


class GroupPrimitivesComponent : public Usul::Base::Referenced,
                                 public Usul::Interfaces::ICommand,
                                 public Usul::Interfaces::IMenuEntry,
                                 public Usul::Interfaces::IPlugin
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GroupPrimitivesComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  GroupPrimitivesComponent();

protected:    

  /// Destructor
  ~GroupPrimitivesComponent();

  /// Usul::Interfaces::IPlugin implementation
  virtual std::string getPluginName() const;
    
  /// Usul::Interfaces::IMenuEntry
  virtual std::string   menuText() const;
  virtual std::string   hotKeyText() const;
  virtual std::string   statusBarText() const;
  virtual std::string   menuGroup() const;

  /// Usul::Interfaces::ICommand
  virtual void execute ( Usul::Interfaces::IUnknown *caller );
};

#endif /* __GROUPPRIMITIVESCOMPONENT_H__  */		
	
