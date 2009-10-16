
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

#ifndef _VAPOR_INTRUSION_GUI_DELEGATE_COMPONENT_CLASS_H_
#define _VAPOR_INTRUSION_GUI_DELEGATE_COMPONENT_CLASS_H_

#include "CompileGuard.h"
#include "VaporIntrusionGUIViewer.h"
#include "MaterialContainer.h"
#include "MaterialDialog.h"
#include "NewVaporIntrusion.h"
#include "GridEditorDialog.h"
#include "InputParameterDialog.h"
#include "ScalarEditorDialog.h"
#include "BuildingDialog.h"
#include "SourceDialog.h"
#include "ContaminantDialog.h"
#include "AddContaminantsToSourceDialog.h"
#include "SoilDialog.h"
#include "SendDialog.h"
#include "GridSpaceDialog.h"
#include "CracksDialog.h"
#include "ModifyGridPointsDlg.h"

#include "VaporIntrusionGUI/Interfaces/IVPIDelegate.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
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
                                           public Usul::Interfaces::IInitNewDocument,
                                           public VaporIntrusionGUI::Interfaces::IVPIDelegate
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
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVaporIntrusionGUI;
  typedef IVaporIntrusionGUI::Category Category;
  typedef IVaporIntrusionGUI::Categories Categories;
  typedef IVaporIntrusionGUI::MaterialsMap MaterialsMap;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::Building Building;

  // useful typedefs
  typedef std::map< std::string, unsigned int > DockMap;
  typedef std::vector< QDockWidget* > DockList;
  typedef std::map< std::string, MaterialContainer* > MaterialContainers;


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

  // edit the scalar values
  void                        editScalar();

  // edit the Building values
  void                        editBuilding();

  // edit the Source values
  void                        editSources();

  // edit the Contaminant values
  void                        editContaminants();

  // edit the Contaminant values
  void                        editAddContaminantsToSource();

  // edit the Soil values
  void                        editSoils();

  // edit the Soil values
  void                        editCracks();

  // send the file to the server
  void                        sendFile();

  // end the grid axis points
  void                        editGridAxisPoints();

  // edit the input parameters
  void                        editInputParameters( const std::string& menuName );

  // IVPIDelegate
  virtual void                addDock( const std::string& name );
  virtual void                addToDock( const std::string& dockName, const std::string& name, const std::string& value );

protected: 

  // Do not copy.
  VaporIntrusionGUIDelegateComponent ( const VaporIntrusionGUIDelegateComponent & );
  VaporIntrusionGUIDelegateComponent &operator = ( const VaporIntrusionGUIDelegateComponent & );

  /// Use reference counting.
  virtual ~VaporIntrusionGUIDelegateComponent();

private:
  QtViewerPtr                         _xyzView;
  DockMap                             _dockMap;
  DockList                            _dockList;
  MaterialContainers                  _materialContainers;
  IUnknown::QueryPtr                  _caller;

  IVaporIntrusionGUI::Categories      _categories;
  IVaporIntrusionGUI::GridMaterials   _gridMaterials;


};


#endif // _VAPOR_INTRUSION_GUI_DELEGATE_COMPONENT_CLASS_H_
