
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CALCULATE_ENERGY_COMPONENT_CLASS_H_
#define _CALCULATE_ENERGY_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IMenuEntry.h"
#include "Usul/Interfaces/IPlugin.h"


namespace OsgFox {
namespace Plugins {
namespace CalculateEnergy {


class Component : public Usul::Base::Referenced,
                  public Usul::Interfaces::IMenuEntry,
                  public Usul::Interfaces::ICommand,
                  public Usul::Interfaces::IPlugin

{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Component );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Component();

  ///  Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const { return "Calculate Energy"; }

  ///  Usul::Interfaces::ICommand
  virtual void execute ( Usul::Interfaces::IUnknown *caller );

  ///  Usul::Interfaces::IMenuEntry
  virtual std::string     menuText() const;
  virtual std::string     hotKeyText() const;
  virtual std::string     statusBarText() const;
  virtual std::string     menuGroup() const;

protected: 

  // Do not copy.
  Component ( const Component & );
  Component &operator = ( const Component & );

  /// Use reference counting.
  virtual ~Component();

};

}; // namespace CalculateEnergy
}; // namespace Plugins
}; // namespace OsgFox


#endif // _CALCULATE_ENERGY_COMPONENT_CLASS_H_
