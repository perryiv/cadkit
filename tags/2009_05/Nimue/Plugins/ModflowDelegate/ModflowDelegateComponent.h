
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_DELEGATE_COMPONENT_CLASS_H_
#define _MODFLOW_DELEGATE_COMPONENT_CLASS_H_

#include "ModflowDelegate/CompileGuard.h"

#include "ModflowModel/Interfaces/IModflowModel.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/IInitNewDocument.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IPluginInitialize.h"
#include "Usul/Interfaces/IStringGridSet.h"

#include <map>
#include <string>

namespace QtTools { class TreeControl; class AnimationControl; }
namespace Modflow { class Properties; }

class QDockWidget;
class QWidget;
class QTextEdit;


class ModflowDelegateComponent : public Usul::Base::Object,
                                 public Usul::Interfaces::IPlugin,
                                 public Usul::Interfaces::IGUIDelegate,
                                 public Usul::Interfaces::IActiveDocumentListener,
																 public Usul::Interfaces::IPluginInitialize,
                                 public Usul::Interfaces::IStringGridSet,
                                 public Usul::Interfaces::IInitNewDocument
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef std::map < QWidget *, QDockWidget * > DockedMap;
  typedef Usul::Interfaces::IStringGridSet IStringGridSet;
  typedef IStringGridSet::StringRow StringRow;
  typedef IStringGridSet::StringGrid StringGrid;
  typedef Modflow::Interfaces::IModflowModel IModflowModel;

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
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Unknown* caller );

  // The active document has changed (IActiveDocumentListener).
  virtual void                activeDocumentChanged ( Unknown *oldDoc, Unknown *newDoc );

  /// Add a dock window (IPluginInitialize).
  virtual void                initializePlugin ( Unknown *caller = 0x0 );

  // Usul::Interfaces::IStringGridSet.
  virtual void                setStringGrid ( const StringGrid &grid );

  // Usul::Interfaces::IInitNewDocument.
  virtual bool                handlesDocumentType ( Unknown *document );
  virtual void                initNewDocument ( Unknown *document, Unknown *caller );

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
  QtTools::TreeControl *_layerTree;
  QtTools::AnimationControl *_animation;
  QTextEdit *_intersectInfo;
  Modflow::Properties *_properties;
  IModflowModel::QueryPtr _model;
};


#endif // _MODFLOW_DELEGATE_COMPONENT_CLASS_H_
