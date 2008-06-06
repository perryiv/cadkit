
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ATOM_INFO_COMPONENT_CLASS_H_
#define _ATOM_INFO_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "FoxTools/Headers/Object.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IFoxEvent.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IMenuEntry.h"
#include "Usul/Interfaces/IPlugin.h"

namespace osgUtil { class Hit; }
namespace osg { class Geode; class Node; }
namespace osgPlugins { namespace pdb { class Atom; } }


namespace OsgFox {
namespace Plugins {
namespace AtomInfo {


class Component : public Usul::Base::Referenced,
                  public Usul::Interfaces::IFoxEvent,
                  public Usul::Interfaces::IMenuEntry,
                  public Usul::Interfaces::ICommand,
                  public Usul::Interfaces::IPlugin,
                  public FX::FXObject

{
  FXDECLARE( Component );
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Component );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Component();

  enum
  {
    ID_FIX_ATOM = 1
  };

  long onCommandFixAtom  ( FX::FXObject *, FX::FXSelector, void * );

  ///  Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const { return "Atom Information"; }

  ///  Usul::Interfaces::ICommand
  virtual void execute ( Usul::Interfaces::IUnknown *caller );

  ///  Usul::Interfaces::IFoxEvent
  virtual bool execute( Unknown* caller, const FX::FXEvent &event);

  ///  Usul::Interfaces::IMenuEntry
  virtual std::string     menuText() const;
  virtual std::string     hotKeyText() const;
  virtual std::string     statusBarText() const;
  virtual std::string     menuGroup() const;

protected: 

  bool _handleLeftClick  ( Unknown *caller, const FX::FXEvent &event );
  bool _handleMouseMove  ( Unknown *caller, const FX::FXEvent &event );
  bool _handleRightClick ( Unknown *caller, const FX::FXEvent &event );

  std::string            _getAtomInfo( osgUtil::Hit& );
  osgPlugins::pdb::Atom* _getAtom( osg::Geode* );
  osg::Node*             _getScene( Unknown* caller );

  // Do not copy.
  Component ( const Component & );
  Component &operator = ( const Component & );

  /// Use reference counting.
  virtual ~Component();

  Usul::Interfaces::IUnknown::ValidAccessQueryPtr _caller;
};

}; // namespace Seek
}; // namespace Plugins
}; // namespace OsgFox


#endif // _ATOM_INFO_COMPONENT_CLASS_H_
