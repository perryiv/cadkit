
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FIELD_VIEW_READER_DELEGATE_COMPONENT_CLASS_H_
#define _FIELD_VIEW_READER_DELEGATE_COMPONENT_CLASS_H_

#include "CompileGuard.h"
#include "VaporIntrusionGUIViewer.h"
#include "MaterialContainer.h"
#include "MaterialDialog.h"
#include "NewVaporIntrusion.h"
#include "GridEditorDialog.h"
#include "InputParameterDialog.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IPluginInitialize.h"
#include "Usul/Interfaces/IInitNewDocument.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include <string>

class QDockWidget;

class VaporIntrusionGUIDelegateComponent : public Usul::Base::Object,
                                           public Usul::Interfaces::IPlugin,
                                           public Usul::Interfaces::IGUIDelegate,
                                           public Usul::Interfaces::IMenuAdd,
                                           public Usul::Interfaces::IPluginInitialize,
                                           public Usul::Interfaces::IInitNewDocument
{
public:

  /// Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef VaporIntrusionGUIViewer VIGUIViewer;
  typedef CadKit::Helios::Views::OSG::Viewer QtViewer;

  // Non-ref'ing smart-pointers that throw if given null.
  typedef Usul::Pointers::Configs::NoRefCountingNullOk Policy;
  typedef Usul::Pointers::SmartPointer < QtViewer, Policy > QtViewerPtr;
  typedef Usul::Pointers::SmartPointer < VIGUIViewer, Policy > VIGUIViewerPtr;

  // IVaporIntrusionGUI typedefs
  typedef Usul::Interfaces::IVaporIntrusionGUI IVaporIntrusionGUI;
  typedef IVaporIntrusionGUI::Category Category;
  typedef IVaporIntrusionGUI::Categories Categories;


  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VaporIntrusionGUIDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  VaporIntrusionGUIDelegateComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Vapor Intrusion GUI Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

  //Usul::Interfaces::IMenuAdd
  virtual void                menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );
 
  //  Usul::Interfaces::IPluginInitialize
  virtual void initializePlugin ( Usul::Interfaces::IUnknown *caller = 0x0 );

  // Usul::Interfaces::IInitNewDocument.
  virtual bool                handlesDocumentType ( Unknown *document );
  virtual void                initNewDocument ( Unknown *document, Unknown *caller );

  // edit the grid
  void                        editGrid();

  // edit the input parameters
  void                        editInputParameters( const std::string& menuName );

protected: 

  // Do not copy.
  VaporIntrusionGUIDelegateComponent ( const VaporIntrusionGUIDelegateComponent & );
  VaporIntrusionGUIDelegateComponent &operator = ( const VaporIntrusionGUIDelegateComponent & );

  /// Use reference counting.
  virtual ~VaporIntrusionGUIDelegateComponent();

private:
  QtViewerPtr         _xyzView;
  VIGUIViewerPtr      _xyView;
  VIGUIViewerPtr      _xzView;
  VIGUIViewerPtr      _yzView;

  QDockWidget         *_dock;
  MaterialContainer   *_materialContainer;
  IUnknown::QueryPtr  _caller;

  Usul::Interfaces::IVaporIntrusionGUI::Categories _categories;


};


#endif // _FIELD_VIEW_READER_DELEGATE_COMPONENT_CLASS_H_
