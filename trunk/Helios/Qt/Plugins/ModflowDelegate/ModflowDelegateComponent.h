
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

#ifndef _MODFLOW_DELEGATE_COMPONENT_CLASS_H_
#define _MODFLOW_DELEGATE_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/GUI/IAddDockWindow.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/IStringGridSet.h"

#include <map>
#include <string>

class LayerTreeControl;
class QDockWidget;
class QWidget;
class QLabel;


class ModflowDelegateComponent : public Usul::Base::Object,
                                 public Usul::Interfaces::IPlugin,
                                 public Usul::Interfaces::IGUIDelegate,
                                 public Usul::Interfaces::IActiveDocumentListener,
                                 public Usul::Interfaces::IAddDockWindow,
                                 public Usul::Interfaces::IStringGridSet
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef std::map < QWidget *, QDockWidget * > DockedMap;
  typedef Usul::Interfaces::IStringGridSet IStringGridSet;
  typedef IStringGridSet::StringRow StringRow;
  typedef IStringGridSet::StringGrid StringGrid;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ModflowDelegateComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  ModflowDelegateComponent();

  // Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Modflow Delegate"; }

  // Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle ( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

  // The active document has changed (IActiveDocumentListener).
  virtual void                activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc );

  /// Add a dock window (IAddDockWindow).
  virtual void                addDockWindow ( Usul::Interfaces::IUnknown *caller = 0x0 );

  // Usul::Interfaces::IStringGridSet.
  virtual void                setStringGrid ( const StringGrid &grid );

protected: 

  // Do not copy.
  ModflowDelegateComponent ( const ModflowDelegateComponent & );
  ModflowDelegateComponent &operator = ( const ModflowDelegateComponent & );

  // Use reference counting.
  virtual ~ModflowDelegateComponent();

private:

  void                        _destroy();

  QDockWidget *               _makeDockWindow ( const std::string &title, const std::string &name );

  Unknown::QueryPtr _caller;
  DockedMap _docked;
  LayerTreeControl *_layerTree;
  QLabel *_intersectInfo;
};


#endif // _MODFLOW_DELEGATE_COMPONENT_CLASS_H_
