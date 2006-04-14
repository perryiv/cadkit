
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SELECTION_STL_COMPONENT_CLASS_H_
#define _SELECTION_STL_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "FoxTools/Headers/Object.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IMenuEntry.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IFoxEvent.h"
#include "Usul/Interfaces/IFoxToolbar.h"
#include "Usul/Interfaces/ISetCursor.h"
#include "Usul/Interfaces/ICleanUp.h"
#include "Usul/Interfaces/IPlugin.h"

#include "osg/Vec3"
#include "osg/Polytope"

#include <memory>

namespace osg { class Geode; }
namespace FX { struct FXEvent; }


namespace Selection {
namespace STL {


class SelectionObject : public Usul::Base::Referenced,
                        public Usul::Interfaces::IMenuEntry,
                        public Usul::Interfaces::ICommand,
                        public Usul::Interfaces::IFoxEvent,
                        public Usul::Interfaces::IFoxToolbar,
                        public Usul::Interfaces::ISetCursor,
                        public Usul::Interfaces::ICleanUp,
                        public Usul::Interfaces::IPlugin,
                        public FX::FXObject
{
  FXDECLARE ( SelectionObject );
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  enum
  {
    ID_SELECTION = 1
  };

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SelectionObject );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  SelectionObject();

  long onCommandSelection  ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateSelection   ( FX::FXObject *, FX::FXSelector, void * );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IPlugin
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual std::string getPluginName() const { return "Selection STL"; }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IMenuEntry
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the menu text.
  virtual std::string     menuText() const;

  // Get the hot-key text.
  virtual std::string     hotKeyText() const;

  // Get the status-bar text.
  virtual std::string     statusBarText() const;

  // Get the group for this menu entry
  virtual std::string   menuGroup() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ICommand
  //
  /////////////////////////////////////////////////////////////////////////////

  // Execute the command.
  virtual void           execute ( Usul::Interfaces::IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IFoxEvent
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual bool execute( Unknown* caller, const FX::FXEvent &event);

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IFoxToolBar
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void addButtons( Usul::Interfaces::IUnknown* caller, FoxTools::ToolBar::Bar* );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ISetCursor
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual FX::FXCursor* getCursor();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ICleanUp
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void cleanUp( Usul::Interfaces::IUnknown* caller );

protected:

  // Do not copy.
  SelectionObject ( const SelectionObject & );
  SelectionObject &operator = ( const SelectionObject & );

  /// Use reference counting.
  virtual ~SelectionObject();

  void _selectionVerticesGeode( osg::Geode *);
  bool _containsPoint( const osg::Vec3& );
  bool _containsTriangle ( const osg::Vec3&, const osg::Vec3&, const osg::Vec3& );
  void _setCorners();

private:
  IUnknown::ValidAccessQueryPtr _caller;
  osg::Vec3 _cornerOne, _cornerTwo;
  std::auto_ptr< FX::FXCursor> _selectionCursor;

  osg::Polytope *_frustum;

};


}; // namespace STL
}; // namespace Selection


#endif // _SELECTION_STL_COMPONENT_CLASS_H_
