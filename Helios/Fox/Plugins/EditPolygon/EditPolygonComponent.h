
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The EditPolygonComponent class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _EDIT_POLYGON_COMPONENT_CLASS_H_
#define _EDIT_POLYGON_COMPONENT_CLASS_H_

#include "CompileGuard.h"
#include "CapPolygons.h"

#include "FoxTools/Headers/Object.h"
#include "FoxTools/Menu/Group.h"
#include "FoxTools/ToolBar/ToggleButton.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IFoxSubMenu.h"
#include "Usul/Interfaces/IFoxToolbar.h"
#include "Usul/Interfaces/IFoxEvent.h"
#include "Usul/Interfaces/IPlugin.h"

namespace Usul { namespace Interfaces { struct IActiveView; } }

namespace Helios {
namespace Plugins {
namespace EditPolygons {

class EditPolygonComponent : public Usul::Base::Referenced,
                  public Usul::Interfaces::IFoxSubMenu,
                  public Usul::Interfaces::IFoxToolbar,
                  public Usul::Interfaces::IPlugin,
                  public FX::FXObject
{
FXDECLARE ( EditPolygonComponent )
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( EditPolygonComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  EditPolygonComponent();

  ///  Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const { return "Edit Polygon"; }

  ///  Usul::Interfaces::IFoxSubMenu  
  virtual void buildSubMenu( Usul::Interfaces::IUnknown *caller, FoxTools::Menu::Group *menuPane  );

  ///  Usul::Interfaces::IFoxToolbar
  virtual void addButtons( Usul::Interfaces::IUnknown* caller, FoxTools::ToolBar::Bar* );

  enum
    {
      ID_DELETE_PRIMITIVE = 1,
      ID_DELETE_GEOMETRY,
      ID_KEEP_CONNECTED,
      ID_CAP_POLYGONS,
      ID_ADD_TRIANGLE,
      ID_FLIP_NORMAL,
      ID_LAST
    };

    long  onCommandDeletePrimitive       ( FX::FXObject *, FX::FXSelector, void * );
    long  onCommandDeleteGeometry        ( FX::FXObject *, FX::FXSelector, void * );
    long  onCommandKeepConnected         ( FX::FXObject *, FX::FXSelector, void * );
    long  onCommandCapPolygons           ( FX::FXObject *, FX::FXSelector, void * );
    long  onCommandAddTriangle           ( FX::FXObject *, FX::FXSelector, void * );
    long  onCommandFlipNormal            ( FX::FXObject *, FX::FXSelector, void * );

    long  onUpdateDeletePrimitive        ( FX::FXObject *, FX::FXSelector, void * );
    long  onUpdateDeleteGeometry         ( FX::FXObject *, FX::FXSelector, void * );
    long  onUpdateKeepConnected          ( FX::FXObject *, FX::FXSelector, void * );
    long  onUpdateCapPolygons            ( FX::FXObject *, FX::FXSelector, void * );
    long  onUpdateAddTriangle            ( FX::FXObject *, FX::FXSelector, void * );
    long  onUpdateFlipNormal             ( FX::FXObject *, FX::FXSelector, void * );

protected:

  // Do not copy.
  EditPolygonComponent ( const EditPolygonComponent & );
  EditPolygonComponent &operator = ( const EditPolygonComponent & );

  /// Use reference counting.
  virtual ~EditPolygonComponent();

private:

  long _onCommand ( Usul::Interfaces::IFoxEvent * );
  long _onUpdate  ( FX::FXObject*, Usul::Interfaces::IFoxEvent * );

  void _init   ( Usul::Interfaces::IUnknown *caller );

  Usul::Interfaces::IUnknown::ValidAccessQueryPtr _caller;

  Usul::Interfaces::IFoxEvent::ValidAccessQueryPtr _deletePrimitive;
  Usul::Interfaces::IFoxEvent::ValidAccessQueryPtr _deleteGeometry;
  Usul::Interfaces::IFoxEvent::ValidAccessQueryPtr _keepConnected;
  Usul::Interfaces::IFoxEvent::ValidAccessQueryPtr _addTriangle;
  Usul::Interfaces::IFoxEvent::ValidAccessQueryPtr _flipNormal;

  EditPolygons::CapPolygons::ValidAccessRefPtr _capPolygons;

};

} // namespace EditPolygons
} // namespace Plugins
} // namespace Helios


#endif // _EDIT_POLYGON_COMPONENT_CLASS_H_

