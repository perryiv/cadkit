
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

namespace Usul {

  namespace Documents { class Document; }

namespace App {

class USUL_EXPORT Controller
{
public:
  Controller(void);
  ~Controller(void);

  typedef Usul::Interfaces::IPolygonMode IPolygonMode;
  typedef Usul::Interfaces::IShadeModel  IShadeModel;
  typedef Usul::Documents::Document Document;

  Document *                 activeDocument();
  Usul::Interfaces::IViewer* activeView();

  void boundingBoxToggle();
  void boundingSphereToggle();

  bool hasPolygonMode ( IPolygonMode::Mode mode );
  void setPolygonMode ( IPolygonMode::Mode mode );

  bool polygonModeEnable ();

  bool hasShadeModel ( IShadeModel::Mode mode );
  void setShadeModel ( IShadeModel::Mode mode );

  bool shadeModelEnable ();
};

}
}

#endif // __USUL_APP_CONTROLLER_H__

