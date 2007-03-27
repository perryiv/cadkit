
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_APP_CONTROLLER_H__
#define __USUL_APP_CONTROLLER_H__

#include "Usul/Export/Export.h"
#include "Usul/Interfaces/IViewer.h"
#include "Usul/Interfaces/IPolygonMode.h"
#include "Usul/Interfaces/IShadeModel.h"

#include <vector>
#include <string>

namespace Usul {

  namespace Documents { class Document; }

namespace App {

class USUL_EXPORT Controller
{
public:

  // Typedefs
  typedef Usul::Interfaces::IPolygonMode           IPolygonMode;
  typedef Usul::Interfaces::IShadeModel            IShadeModel;
  typedef Usul::Documents::Document                Document;
  typedef std::pair<std::string,std::string>       Filter;
  typedef std::vector<Filter>                      Filters;
  typedef std::pair < unsigned int, std::string >  StereoMode;
  typedef std::vector < StereoMode >               StereoModes;

  // Singleton.
  static Controller &         instance();

  Document *                  activeDocument();
  Usul::Interfaces::IViewer*  activeView();

  std::string                 aboutMessage ( );

  bool                        axesCheck ();
  bool                        axesEnable ();
  void                        axesShow ( bool b );

  void                        backgroundDefault();
  void                        backgroundEdit();
  bool                        backgroundEnable();

  void                        boundingBoxToggle();
  void                        boundingSphereToggle();

  bool                        boundingBoxEnable ();
  bool                        boundingBoxCheck  ();

  bool                        boundingSphereEnable ();
  bool                        boundingSphereCheck  ();

  void                        camera ( unsigned int option );
  bool                        cameraEnable ();

  void                        centerOfRotationSet ( bool b );
  bool                        centerOfRotationCheck ();

  bool                        displayLists();
  void                        displayListsToggle();

  void                        documentOpen ( const std::string& filename, Usul::Interfaces::IUnknown *caller );
  void                        documentInsert ( Usul::Interfaces::IUnknown* caller );
  void                        documentSave   ( Usul::Interfaces::IUnknown* caller );
  void                        documentSaveAs ( Usul::Interfaces::IUnknown* caller );
  void                        documentExport ( const std::string& filename, std::string filter, Usul::Interfaces::IUnknown* caller );
  void                        documentNew ( Usul::Interfaces::IUnknown* component, Usul::Interfaces::IUnknown* caller );

  void                        filtersExport ( Filters &filters );

  void                        flipNormals ( Usul::Interfaces::IUnknown* caller );
  bool                        flipNormalsEnable ( );

  void                        highLods();
  bool                        highLodsCheck();

  void                        loadPlugins();

  void                        lowLods();
  bool                        lowLowsCheck();

  void                        lightingSet ( bool b );
  bool                        lightingCheck ();
  bool                        lightingEnable ();

  void                        lightingTwoSidedSet ( bool b );
  bool                        lightingTwoSidedCheck  ();
  bool                        lightingTwoSidedEnable ();

  void                        lightsShow ( bool );
  bool                        lightsCheck ();
  bool                        lightsEnable ();

  bool                        modeNavigatingCheck ( );
  bool                        modeNavigatingEnable ( );
  void                        modeNavigatingSet ( );

  bool                        modePickingCheck ( );
  bool                        modePickingEnable ( );
  void                        modePickingSet ( );

  void                        newWindow ( Usul::Interfaces::IUnknown* caller );

  bool                        hasPolygonMode ( IPolygonMode::Mode mode );
  void                        setPolygonMode ( IPolygonMode::Mode mode );

  bool                        polygonModeEnable ();

  bool                        hasShadeModel ( IShadeModel::Mode mode );
  void                        setShadeModel ( IShadeModel::Mode mode );

  bool                        sceneStage();
  void                        sceneStage ( bool b );

  bool                        shadeModelEnable ();

  void                        spinToggle ();
  bool                        spinCheck ();

  void                        stereoSet ( bool b );
  bool                        stereoCheck ();
  bool                        stereoCheck ( unsigned int mode );
  bool                        stereoEnable();
  const StereoModes&          stereoModes();
  void                        stereoModeSet ( unsigned int mode );
  void                        stereoEyeDistanceSet ( float distance );
  bool                        stereoEyeDistanceCheck ( float distance );

private:
  Controller  ( );
  ~Controller ( );
  Controller ( const Controller& ); // No copying.
  Controller& operator= ( const Controller& ); // No assignment.

  static Controller *_instance;
};

}
}

#endif // __USUL_APP_CONTROLLER_H__

