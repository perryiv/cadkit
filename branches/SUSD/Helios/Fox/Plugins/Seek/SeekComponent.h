
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

#ifndef _VIEW_SEEK_COMPONENT_CLASS_H_
#define _VIEW_SEEK_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "FoxTools/Headers/Object.h"
#include "FoxTools/ToolBar/ToggleButton.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/ITool.h"
#include "Usul/Interfaces/Fox/IFoxToolbar.h"
#include "Usul/Interfaces/Fox/ISetCursor.h"
#include "Usul/Interfaces/IPlugin.h"

#include <memory>


class SeekComponent : public Usul::Base::Referenced,
                      public Usul::Interfaces::ITool,
                      public Usul::Interfaces::IFoxToolbar,
                      public Usul::Interfaces::ISetCursor,
                      public Usul::Interfaces::IPlugin,
                      public FX::FXObject

{
  FXDECLARE ( SeekComponent );
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SeekComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  SeekComponent();

  enum
  {
    ID_SEEK = 1
  };

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IPlugin
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual std::string getPluginName() const { return "View Seek"; }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ITool
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual bool execute( Unknown* caller,  bool left, bool middle, bool right, bool motion, float x, float y, float z );

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

  long onCommandSeek  ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateSeek   ( FX::FXObject *, FX::FXSelector, void * );
protected:

  // Do not copy.
  SeekComponent ( const SeekComponent & );
  SeekComponent &operator = ( const SeekComponent & );

  /// Use reference counting.
  virtual ~SeekComponent();

  Usul::Interfaces::IUnknown* _mainWindow;
  std::auto_ptr< FX::FXCursor> _seekCursor;
  FoxTools::ToolBar::ToggleButton::ValidAccessRefPtr _seekButton;
};



#endif // _VIEW_SEEK_COMPONENT_CLASS_H_
