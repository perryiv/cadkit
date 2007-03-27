
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

#ifndef _IMD_COMPONENT_CLASS_H_
#define _IMD_COMPONENT_CLASS_H_

#include "CompileGuard.h"
#include "IMD.h"

#include "FoxTools/Headers/Object.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IMenuEntry.h"
#include "Usul/Interfaces/IPlugin.h"

namespace osgUtil { class Hit; }
namespace osg { class Geode; class Node; }

namespace osgPlugins { namespace pdb { class Atom; } }

namespace Usul { namespace Interfaces { struct ITextMatrix;  } }

namespace OsgFox {
namespace Plugins {
namespace IMD {


class IMDComponent : public Usul::Base::Referenced,
                     public Usul::Interfaces::IMenuEntry,
                     public Usul::Interfaces::ICommand,
                     public Usul::Interfaces::IPlugin,
                     public FX::FXObject
{
  FXDECLARE( IMDComponent );
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( IMDComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  IMDComponent();

  enum
  {
    ID_FIX_ATOM = 1
  };

  long onCommandFixAtom  ( FX::FXObject *, FX::FXSelector, void * );

  ///  Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const { return "IMD"; }

  ///  Usul::Interfaces::ICommand
  virtual void execute ( Usul::Interfaces::IUnknown *caller );

  ///  Usul::Interfaces::IMenuEntry
  virtual std::string     menuText() const;
  virtual std::string     hotKeyText() const;
  virtual std::string     statusBarText() const;
  virtual std::string     menuGroup() const;

protected: 

  void _simulate( const std::string& hostName, const std::string& portNumber );

  void _processCoordinates ( int32 length, const OsgFox::Plugins::IMD::IMD& imd );
  void _processEnergies    ( int32 length, const OsgFox::Plugins::IMD::IMD& imd, Usul::Interfaces::ITextMatrix* textMatrix );

  // Do not copy.
  IMDComponent ( const IMDComponent & );
  IMDComponent &operator = ( const IMDComponent & );

  /// Use reference counting.
  virtual ~IMDComponent();

  Usul::Interfaces::IUnknown *_caller;
};

}; // namespace IMD
}; // namespace Plugins
}; // namespace OsgFox


#endif // _ATOM_INFO_COMPONENT_CLASS_H_
